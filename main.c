/**
 * Morse Code CLI Application
 *
 * A pure functional implementation of a Morse code encoder/decoder
 * following ADT principles.
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Type definitions for functional programming approach */
typedef struct {
  int argc;
  char **argv;
} ProgramArgs;

typedef struct {
  bool help;
  bool programmerInfo;
  bool decode;
  bool encode;
  char *inputText;
  char *inputFile;
  char *outputFile;
  bool readFromPipe;
} Options;

typedef struct {
  char character;
  const char *code;
} MorseMapping;

/* Immutable morse code mapping table */
static const MorseMapping MORSE_TABLE[] = {
    {'A', ".-"},     {'B', "-..."},   {'C', "-.-."},   {'D', "-.."},
    {'E', "."},      {'F', "..-."},   {'G', "--."},    {'H', "...."},
    {'I', ".."},     {'J', ".---"},   {'K', "-.-"},    {'L', ".-.."},
    {'M', "--"},     {'N', "-."},     {'O', "---"},    {'P', ".--."},
    {'Q', "--.-"},   {'R', ".-."},    {'S', "..."},    {'T', "-"},
    {'U', "..-"},    {'V', "...-"},   {'W', ".--"},    {'X', "-..-"},
    {'Y', "-.--"},   {'Z', "--.."},   {'0', "-----"},  {'1', ".----"},
    {'2', "..---"},  {'3', "...--"},  {'4', "....-"},  {'5', "....."},
    {'6', "-...."},  {'7', "--..."},  {'8', "--.."},   {'9', "---.."},
    {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."}, {'!', "-.-.--"},
    {' ', "/"},      {':', "---..."}, {';', "-.-.-."}, {'=', "-...-"},
    {'-', "-....-"}, {'+', ".-.-."},  {'_', "..--.-"}, {'(', "-.--."},
    {')', "-.--.-"}, {'/', "-..-."},  {'@', ".--.-."}};

/* Function prototypes */
Options parseCommandLine(const ProgramArgs args);

void displayHelp(void);
void displayProgrammerInfo(void);

char *encodeText(const char *text);
char *decodeText(const char *morse);

const char *getCharacterCode(char c);

char getCodeCharacter(const char *code);

char *strdup_safe(const char *str);

bool writeFileContent(const char *filename, const char *content);

char *readFileContent(const char *filename);
char *readFromStdin(void);

bool isInputFromPipe(void);

/* Main function */
int main(int argc, char *argv[]) {
  ProgramArgs args = {argc, argv};
  Options options = parseCommandLine(args);

  if (options.help) {
    displayHelp();
    return 0;
  }

  if (options.programmerInfo) {
    displayProgrammerInfo();
    return 0;
  }

  char *inputContent = NULL;

  // Determine input source and read content
  if (options.readFromPipe) {
    // ReqFunc10: Read from pipe if available
    inputContent = readFromStdin();
  } else if (options.inputFile != NULL) {
    // ReqFunc08: Handle input files with any path
    inputContent = readFileContent(options.inputFile);
    if (inputContent == NULL) {
      // ReqFunc09: Error if file not found
      fprintf(stderr, "Error: Could not read input file '%s'\n",
              options.inputFile);
      return 1;
    }
  } else if (options.inputText != NULL) {
    inputContent = strdup_safe(options.inputText);
  } else {
    fprintf(stderr, "Error: No input text provided.\n");
    displayHelp();
    return 1;
  }

  // Process the input
  char *result;
  if (options.decode) {
    result = decodeText(inputContent);
  } else {
    result = encodeText(inputContent);
  }

  // Output the result
  if (options.outputFile != NULL) {
    // ReqFunc11, ReqFunc12: Write to output file
    if (!writeFileContent(options.outputFile, result)) {
      fprintf(stderr, "Error: Could not write to output file '%s'\n",
              options.outputFile);
      free(result);
      free(inputContent);
      if (options.inputFile)
        free(options.inputFile);
      if (options.outputFile)
        free(options.outputFile);
      return 1;
    }
  } else {
    // Print result to stdout
    if (options.decode) {
      printf("Decoded: %s\n", result);
    } else {
      printf("Encoded: %s\n", result);
    }
  }

  // Cleanup
  free(result);
  free(inputContent);
  if (options.inputFile)
    free(options.inputFile);
  if (options.outputFile)
    free(options.outputFile);

  return 0;
}

/* Parse command line arguments in a functional way */
Options parseCommandLine(const ProgramArgs args) {
  Options options = {false, false, false, false, NULL, NULL, NULL, false};

  for (int i = 1; i < args.argc; i++) {
    if (strcmp(args.argv[i], "-h") == 0 ||
        strcmp(args.argv[i], "--help") == 0) {
      options.help = true;
      return options;
    } else if (strcmp(args.argv[i], "--programmer-info") == 0) {
      options.programmerInfo = true;
      return options;
    } else if (strcmp(args.argv[i], "-d") == 0 ||
               strcmp(args.argv[i], "--decode") == 0) {
      options.decode = true;
    } else if (strcmp(args.argv[i], "-e") == 0 ||
               strcmp(args.argv[i], "--encode") == 0) {
      options.encode = true;
    } else if (strcmp(args.argv[i], "-o") == 0 ||
               strcmp(args.argv[i], "--out") == 0) {
      if (i + 1 < args.argc) {
        options.outputFile = strdup_safe(args.argv[++i]);
      } else {
        fprintf(stderr,
                "Error: Output file path not provided after -o/--out\n");
        exit(EXIT_FAILURE);
      }
    } else if (options.inputFile == NULL && options.inputText == NULL) {
      if (access(args.argv[i], R_OK) == 0) {
        options.inputFile = strdup_safe(args.argv[i]);
      } else {
        options.inputText = strdup_safe(args.argv[i]);
      }
    }
  }

  // Check if both encode and decode are specified (ReqFunc07)
  if (options.encode && options.decode) {
    fprintf(stderr,
            "Error: Cannot specify both encode (-e) and decode (-d) options\n");
    exit(EXIT_FAILURE);
  }

  // If neither encode nor decode is specified, default is encode (ReqFunc05)
  if (!options.decode && !options.encode) {
    options.encode = true;
  }

  // Check if we should read from pipe (ReqFunc10)
  options.readFromPipe = isInputFromPipe();

  return options;
}

/* Display help information */
void displayHelp(void) {
  printf("Morse Code Converter - Help\n");
  printf("============================\n\n");
  printf("A pure functional CLI application to convert text to Morse code and "
         "vice versa.\n\n");
  printf("USAGE:\n");
  printf("  morse [OPTIONS] [INPUT_TEXT|INPUT_FILE]\n\n");
  printf("OPTIONS:\n");
  printf("  -h, --help           Display this help message\n");
  printf("  -e, --encode         Encode text to Morse code (default if not "
         "specified)\n");
  printf("  -d, --decode         Decode Morse code to text\n");
  printf("  -o, --out FILE       Write output to specified file instead of "
         "stdout\n");
  printf("  --programmer-info    Display information about the programmer\n\n");
  printf("NOTES:\n");
  printf("  - If both INPUT_TEXT and INPUT_FILE are not provided, input is "
         "read from stdin\n");
  printf("  - Cannot specify both encode (-e) and decode (-d) options\n");
  printf("  - Input and output files can be specified with relative or "
         "absolute paths\n");
  printf("  - Newlines and carriage returns are ignored in input\n");
  printf(
      "  - Letters are separated by single spaces, words by triple spaces\n");
  printf("  - Unsupported characters are represented as '*' in Morse code "
         "output\n\n");
  printf("EXAMPLES:\n");
  printf("  morse -e \"HELLO WORLD\"                        Encode 'HELLO "
         "WORLD' to Morse code\n");
  printf("  morse \"HELLO WORLD\"                           Same as above "
         "(encode is default)\n");
  printf("  morse -d \".... . .-.. .-.. --- / .-- --- .-. .-.. -..\"    Decode "
         "Morse code\n");
  printf("  cat file.txt | morse -e                        Encode content from "
         "pipe\n");
  printf("  morse -e input.txt                             Encode content of "
         "input.txt\n");
  printf("  morse -d input.morse -o output.txt             Decode and write to "
         "output.txt\n\n");
  printf("SUPPORTED CHARACTERS:\n");
  printf("  - Letters: A-Z (case insensitive)\n");
  printf("  - Numbers: 0-9\n");
  printf("  - Symbols: Space, ., ,, :, ;, ?, !, =, -, +, _, (, ), /, @\n\n");
}

/* Display programmer information in JSON format */
void displayProgrammerInfo(void) {
  printf("{\n");
  printf("  \"name\": \"Diego Rubio Carrera\",\n");
  printf("  \"program\": \"TIK\",\n");
  printf("  \"email\": \"diegorubiocarrera@gmail.com\"\n");
  printf("}\n");
}

/* Check if input is coming from a pipe */
bool isInputFromPipe(void) { return !isatty(STDIN_FILENO); }

/* Read content from stdin in a pure functional way */
char *readFromStdin(void) {
  const size_t CHUNK_SIZE = 1024;
  size_t capacity = CHUNK_SIZE;
  size_t size = 0;
  char *buffer = (char *)malloc(capacity);

  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  char chunk[CHUNK_SIZE];
  size_t bytesRead;

  while ((bytesRead = fread(chunk, 1, CHUNK_SIZE - 1, stdin)) > 0) {
    if (size + bytesRead + 1 > capacity) {
      capacity *= 2;
      char *newBuffer = (char *)realloc(buffer, capacity);
      if (newBuffer == NULL) {
        free(buffer);
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
      }
      buffer = newBuffer;
    }

    memcpy(buffer + size, chunk, bytesRead);
    size += bytesRead;
  }

  buffer[size] = '\0';

  // Trim trailing newline if present
  if (size > 0 && buffer[size - 1] == '\n') {
    buffer[size - 1] = '\0';
  }

  return buffer;
}

/* Read file content in a pure functional way */
char *readFileContent(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return NULL;
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate buffer with enough space for the null terminator
  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL) {
    fclose(file);
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  // Read file content
  size_t bytesRead = fread(buffer, 1, fileSize, file);
  fclose(file);

  if (bytesRead < (size_t)fileSize) {
    free(buffer);
    return NULL;
  }

  buffer[bytesRead] = '\0';

  // Trim trailing newline if present
  if (bytesRead > 0 && buffer[bytesRead - 1] == '\n') {
    buffer[bytesRead - 1] = '\0';
  }

  return buffer;
}

/* Write content to file in a pure functional way */
bool writeFileContent(const char *filename, const char *content) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    return false;
  }

  size_t contentLength = strlen(content);
  size_t bytesWritten = fwrite(content, 1, contentLength, file);
  fclose(file);

  return bytesWritten == contentLength;
}

/* Safe string duplication function */
char *strdup_safe(const char *str) {
  if (str == NULL)
    return NULL;

  char *duplicate = strdup(str);
  if (duplicate == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  return duplicate;
}

/* Get Morse code for a character */
const char *getCharacterCode(char c) {
  const int tableSize = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);
  c = toupper(c);

  for (int i = 0; i < tableSize; i++) {
    if (MORSE_TABLE[i].character == c) {
      return MORSE_TABLE[i].code;
    }
  }

  return NULL; // Character not found
}

/* Get character from Morse code */
char getCodeCharacter(const char *code) {
  const int tableSize = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

  for (int i = 0; i < tableSize; i++) {
    if (strcmp(MORSE_TABLE[i].code, code) == 0) {
      return MORSE_TABLE[i].character;
    }
  }

  return '\0'; // Code not found
}

/*
 * Encode text to Morse code
 * Pure functional implementation that creates a new string
 * Implements ReqFunc13-21, ReqFunc23, ReqFunc25-28
 */
char *encodeText(const char *text) {
  if (text == NULL)
    return strdup_safe("");

  // Calculate required buffer size (worst case estimate)
  // Each character could be replaced with "*" or a long morse code sequence
  // Plus spaces between characters and triple spaces between words
  size_t maxBufferSize = strlen(text) * 10;
  char *result = (char *)malloc(maxBufferSize);
  if (result == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  result[0] = '\0';

  bool lastWasSpace = false; // Track spaces for word separation
  bool firstChar = true;     // Track if this is the first character

  // Build the encoded string
  for (size_t i = 0; text[i] != '\0'; i++) {
    char currentChar = text[i];

    // ReqFunc28: Skip newlines and carriage returns
    if (currentChar == '\n' || currentChar == '\r') {
      continue;
    }

    // Handle word separation (ReqFunc27)
    if (currentChar == ' ') {
      if (!lastWasSpace && !firstChar) {
        // Replace single space with three spaces between words
        strcat(result, "   ");
      }
      lastWasSpace = true;
      continue;
    }

    // Add space between letters (except before the first letter) (ReqFunc26)
    if (!firstChar && !lastWasSpace) {
      strcat(result, " ");
    }

    const char *code = getCharacterCode(currentChar);
    if (code != NULL) {
      strcat(result, code);
    } else {
      // ReqFunc25: Output * for characters not in the table
      strcat(result, "*");
    }

    lastWasSpace = false;
    if (firstChar) {
      firstChar = false;
    }
  }

  return result;
}

/*
 * Decode Morse code to text
 * Pure functional implementation that creates a new string
 * Implements ReqFunc14, ReqFunc16, ReqFunc18, ReqFunc20, ReqFunc22, ReqFunc24
 */
char *decodeText(const char *morse) {
  if (morse == NULL)
    return strdup_safe("");

  // Calculate maximum output size (worst case: each code becomes 1 char)
  size_t maxOutputSize = strlen(morse) + 1;
  char *result = (char *)malloc(maxOutputSize);
  if (result == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  result[0] = '\0';

  // Temporary buffer to hold a single morse code character
  char codeBuf[20] = {0};
  size_t codeBufIndex = 0;
  size_t resultIndex = 0;

  // Keep track of consecutive spaces for word separation
  int spaceCount = 0;

  for (size_t i = 0; morse[i] != '\0'; i++) {
    // Skip newlines and carriage returns (ReqFunc28)
    if (morse[i] == '\n' || morse[i] == '\r') {
      continue;
    }

    if (morse[i] == ' ') {
      // End of a morse character or part of word separation
      spaceCount++;

      if (spaceCount == 1 && codeBufIndex > 0) {
        // End of a character (single space)
        codeBuf[codeBufIndex] = '\0';
        char c = getCodeCharacter(codeBuf);
        if (c != '\0') {
          result[resultIndex++] = c;
          result[resultIndex] = '\0';
        }
        codeBufIndex = 0;
      } else if (spaceCount == 3) {
        // End of a word (triple space)
        result[resultIndex++] = ' ';
        result[resultIndex] = '\0';
        spaceCount = 0; // Reset space counter
      }
    } else if (morse[i] == '/') {
      // Alternative notation for space between words
      if (codeBufIndex > 0) {
        codeBuf[codeBufIndex] = '\0';
        char c = getCodeCharacter(codeBuf);
        if (c != '\0') {
          result[resultIndex++] = c;
          result[resultIndex] = '\0';
        }
        codeBufIndex = 0;
      }
      result[resultIndex++] = ' ';
      result[resultIndex] = '\0';
      spaceCount = 0;
    } else {
      // Part of a morse character (dot, dash, or other)
      spaceCount = 0; // Reset space counter
      if (codeBufIndex < sizeof(codeBuf) - 1) {
        codeBuf[codeBufIndex++] = morse[i];
      }
    }
  }

  // Process the last code if there is one
  if (codeBufIndex > 0) {
    codeBuf[codeBufIndex] = '\0';
    char c = getCodeCharacter(codeBuf);
    if (c != '\0') {
      result[resultIndex++] = c;
      result[resultIndex] = '\0';
    }
  }

  return result;
}
