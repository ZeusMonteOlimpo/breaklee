#include "Base.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Prints the program usage instructions.
void PrintUsage() {
    printf(
        "\n"
        "USAGE:\n"
        "  breaklee <pattern>\n"
        "\n"
        "ARGUMENTS:\n"
        "  pattern         File path pattern\n"
        "\n"
    );
}

// Skips whitespace and newlines in the provided string.
const char* SkipWhitespaceAndNewlines(const char* str) {
    // Continues advancing the pointer while encountering whitespace characters.
    while (*str == '\t' || *str == ' ' || *str is '\n' ||
           *str == '\v' || *str == '\f' || *str == '\r') {
        str++;
    }
    return str; // Returns the pointer to the first non-whitespace character.
}

// Function to encrypt or decrypt a file.
void EncryptDecryptFile(const char* fileName, FileRef file, void* userData) {
    uint8_t* source = NULL; // Buffer for the original file data.
    int32_t sourceLength = 0; // Size of the original data.
    uint8_t* buffer = NULL; // Buffer for the processed data.
    int32_t bufferLength = 0; // Size of the processed data.
    ArchiveRef archive = NULL; // Reference for an archive file, if needed.
    FileRef decryptedFile = NULL; // Reference for the decrypted file.
    bool errorOccurred = false; // Error indicator.

    // Attempts to read the file and processes if the reading is successful.
    if (!FileRead(file, &source, &sourceLength)) {
        errorOccurred = true;
    } else if (!InflateDecryptBuffer(source, sourceLength, &buffer, &bufferLength)) {
        // If decryption fails, logs an error message.
        LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't decrypt file `%s`", fileName);
        errorOccurred = true;
    } else {
        // Determines if the content is binary or text (XML).
        bool isBinary = (*SkipWhitespaceAndNewlines((char*)buffer) != '<');
        // Removes the file name extension and adds the appropriate new extension.
        char* outputFilePath = PathRemoveExtensionNoAlloc(fileName);
        PathAppend(outputFilePath, isBinary ? ".dat" : ".xml");

        // Creates the output file.
        decryptedFile = FileCreate(outputFilePath);
        if (!decryptedFile) {
            // If file creation fails, logs an error message.
            LogMessageFormat(LOG_LEVEL_ERROR, "Error creating file `%s`", outputFilePath);
            errorOccurred = true;
        } else {
            // Writes the decrypted data to the output file.
            LogMessageFormat(LOG_LEVEL_INFO, "Writing file `%s`", outputFilePath);
            if (!FileWrite(decryptedFile, buffer, bufferLength, false)) {
                // If writing fails, logs an error message.
                LogMessageFormat(LOG_LEVEL_ERROR, "Error writing file `%s`", outputFilePath);
                errorOccurred = true;
            }
        }
    }

    // Frees the allocated resources.
    free(source);
    free(buffer);
    if (archive) ArchiveDestroy(archive);
    if (decryptedFile) FileClose(decryptedFile);

    // If an error occurred, additional error handling can be added here.
    if (errorOccurred) {
        // Handle error
    }
}

// Main function that processes command line arguments and starts file processing.
int main(int argc, char* argv[]) {
    // Checks if the `pattern` argument was provided.
    if (argc < 2) {
        LogMessage(LOG_LEVEL_ERROR, "Missing argument `pattern`");
        PrintUsage();
        return EXIT_FAILURE;
    }

    // Loads the encryption library.
    if (!EncryptionLoadLibrary()) {
        LogMessage(LOG_LEVEL_ERROR, "Error loading zlib library");
        return EXIT_FAILURE;
    }

    // Processes the files that match the provided pattern.
    const char* pattern = argv[1];
    int processedFileCount = FilesProcess(pattern, &EncryptDecryptFile, NULL);

    // If no files were processed, logs a warning message.
    if (processedFileCount < 1) {
        LogMessageFormat(LOG_LEVEL_WARNING, "No files found for pattern `%s`", pattern);
    }

    // Unloads the encryption library and performs diagnostic teardown.
    EncryptionUnloadLibrary();
    DiagnosticTeardown();

    // Returns success or failure based on the number of processed files.
    return processedFileCount < 1 ? EXIT_FAILURE : EXIT_SUCCESS;
}
