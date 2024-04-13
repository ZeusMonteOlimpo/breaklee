#include "Base.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

const char* SkipWhitespaceAndNewlines(const char* str) {
    while (*str == '\t' || *str == ' ' || *str == '\n' ||
           *str == '\v' || *str == '\f' || *str == '\r') {
        str++;
    }
    return str;
}

void EncryptDecryptFile(const char* fileName, FileRef file, void* userData) {
    uint8_t* source = NULL;
    int32_t sourceLength = 0;
    uint8_t* buffer = NULL;
    int32_t bufferLength = 0;
    ArchiveRef archive = NULL;
    FileRef decryptedFile = NULL;
    bool errorOccurred = false;

    if (!FileRead(file, &source, &sourceLength)) {
        errorOccurred = true;
    } else if (!InflateDecryptBuffer(source, sourceLength, &buffer, &bufferLength)) {
        LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't decrypt file `%s`", fileName);
        errorOccurred = true;
    } else {
        bool isBinary = (*SkipWhitespaceAndNewlines((char*)buffer) != '<');
        char* outputFilePath = PathRemoveExtensionNoAlloc(fileName);
        PathAppend(outputFilePath, isBinary ? ".dat" : ".xml");

        decryptedFile = FileCreate(outputFilePath);
        if (!decryptedFile) {
            LogMessageFormat(LOG_LEVEL_ERROR, "Error creating file `%s`", outputFilePath);
            errorOccurred = true;
        } else {
            LogMessageFormat(LOG_LEVEL_INFO, "Writing file `%s`", outputFilePath);
            if (!FileWrite(decryptedFile, buffer, bufferLength, false)) {
                LogMessageFormat(LOG_LEVEL_ERROR, "Error writing file `%s`", outputFilePath);
                errorOccurred = true;
            }
        }
    }

    free(source);
    free(buffer);
    if (archive) ArchiveDestroy(archive);
    if (decryptedFile) FileClose(decryptedFile);

    if (errorOccurred) {
        // Handle error
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        LogMessage(LOG_LEVEL_ERROR, "Missing argument `pattern`");
        PrintUsage();
        return EXIT_FAILURE;
    }

    if (!EncryptionLoadLibrary()) {
        LogMessage(LOG_LEVEL_ERROR, "Error loading zlib library");
        return EXIT_FAILURE;
    }

    const char* pattern = argv[1];
    int processedFileCount = FilesProcess(pattern, &EncryptDecryptFile, NULL);

    if (processedFileCount < 1) {
        LogMessageFormat(LOG_LEVEL_WARNING, "No files found for pattern `%s`", pattern);
    }

    EncryptionUnloadLibrary();
    DiagnosticTeardown();

    return processedFileCount < 1 ? EXIT_FAILURE : EXIT_SUCCESS;
}

