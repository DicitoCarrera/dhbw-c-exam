/**
 * @file morse.c
 * @brief Morse Code CLI Application
 * @author Diego Rubio Carrera
 *
 * A functional implementation of a Morse code encoder/decoder
 */

#include <ctype.h>
#include <fcntl.h>
#include <getopt.h> // ReqNonFunc05: Required for getopt_long
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Error handling types for functional approach */
typedef enum {
  MORSE_SUCCESS,
  MORSE_INVALID_INPUT,
  MORSE_FILE_NOT_FOUND,
  MORSE_FILE_READ_ERROR,
  MORSE_FILE_WRITE_ERROR,
  MORSE_CONFLICTING_OPTIONS,
  MORSE_MEMORY_ERROR,
  MORSE_INVALID_OPTION
} MorseError;

typedef struct {
  bool hasError;
  MorseError errorCode;
  char *errorMessage;
  void *data;
} Result;

/* Program configuration */
typedef struct {
  bool help;
  bool programmerInfo;
  bool decode;
  bool encode;
  bool slashWordspacer; // ReqOptFunc02
  char *inputText;
  char *inputFile;
  char *outputFile;
  bool readFromPipe;
} Options;

typedef struct {
  char character;
  const char *code;
} MorseMapping;

/* Complete morse code mapping table - ReqFunc13-24 */
static const MorseMapping MORSE_TABLE[] = {
    // Letters A-Z
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.."},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."},
    // Numbers 0-9
    {'0', "-----"},
    {'1', ".----"},
    {'2', "..---"},
    {'3', "...--"},
    {'4', "....-"},
    {'5', "....."},
    {'6', "-...."},
    {'7', "--..."},
    {'8', "---.."},
    {'9', "----."},
    // Punctuation - ReqFunc19-20 (fixed missing entries)
    {'.', ".-.-.-"},
    {',', "--..--"},
    {':', "---..."},
    {';', "-.-.-."},
    {'?', "..--.."},
    {'!', "-.-.--"},
    // Math symbols - ReqFunc21-22
    {'=', "-...-"},
    {'-', "-....-"},
    {'+', ".-.-."},
    // Format symbols - ReqFunc23-24
    {'_', "..--.-"},
    {'(', "-.--."},
    {')', "-.--.-"},
    {'/', "-..-."},
    {'@', ".--.-."},
    // Space handled specially
    {' ', "/"}};

/* Function prototypes */
static Result createSuccess(void *data);
static Result createError(MorseError errorCode, const char *message);
static void freeResult(Result *result);

static Result parseCommandLine(int argc, char **argv);
static void displayHelp(void);
static void displayProgrammerInfo(void);

static Result encodeText(const char *text, bool useSlashWordspacer);
static Result decodeText(const char *morse);

static const char *getCharacterCode(char c);
static char getCodeCharacter(const char *code);

static Result readFileContent(const char *filename);
static Result readFromStdin(void);
static Result writeFileContent(const char *filename, const char *content);

static bool isInputFromPipe(void);
static char *strdup_safe(const char *str);

/* Result helper functions */
static Result createSuccess(void *data) {
  Result result = {false, MORSE_SUCCESS, NULL, data};
  return result;
}

static Result createError(MorseError errorCode, const char *message) {
  Result result = {true, errorCode, NULL, NULL};
  if (message) {
    result.errorMessage = strdup_safe(message);
  }
  return result;
}

static void freeResult(Result *result) {
  if (result && result->errorMessage) {
    free(result->errorMessage);
    result->errorMessage = NULL;
  }
  if (result && result->data) {
    free(result->data);
    result->data = NULL;
  }
}

/* Main function */
int main(int argc, char *argv[]) {
  Result parseResult = parseCommandLine(argc, argv);
  if (parseResult.hasError) {
    if (parseResult.errorMessage) {
      fprintf(stderr, "Error: %s\n", parseResult.errorMessage);
    }
    freeResult(&parseResult);
    return 1;
  }

  Options *options = (Options *)parseResult.data;

  // Handle special options
  if (options->help) {
    displayHelp();
    freeResult(&parseResult);
    return 0;
  }

  if (options->programmerInfo) {
    displayProgrammerInfo();
    freeResult(&parseResult);
    return 0;
  }

  // Determine input source and read content
  Result inputResult;
  if (options->readFromPipe) {
    inputResult = readFromStdin();
  } else if (options->inputFile != NULL) {
    inputResult = readFileContent(options->inputFile);
  } else if (options->inputText != NULL) {
    inputResult = createSuccess(strdup_safe(options->inputText));
  } else {
    fprintf(stderr, "Error: No input text provided.\n");
    displayHelp();
    freeResult(&parseResult);
    return 1;
  }

  if (inputResult.hasError) {
    if (inputResult.errorMessage) {
      fprintf(stderr, "Input Error: %s\n", inputResult.errorMessage);
    }
    freeResult(&parseResult);
    freeResult(&inputResult);
    return 1;
  }

  // Process the input
  Result processResult;
  if (options->decode) {
    // ReqOptFunc03: --slash-wordspacer only allowed with encode
    if (options->slashWordspacer) {
      fprintf(stderr, "Warning: --slash-wordspacer can only be used with "
                      "encode operation\n");
      freeResult(&parseResult);
      freeResult(&inputResult);
      return 1;
    }
    processResult = decodeText((char *)inputResult.data);
  } else {
    processResult =
        encodeText((char *)inputResult.data, options->slashWordspacer);
  }

  if (processResult.hasError) {
    if (processResult.errorMessage) {
      fprintf(stderr, "Processing Error: %s\n", processResult.errorMessage);
    }
    freeResult(&parseResult);
    freeResult(&inputResult);
    freeResult(&processResult);
    return 1;
  }

  // Output the result
  if (options->outputFile != NULL) {
    Result writeResult =
        writeFileContent(options->outputFile, (char *)processResult.data);
    if (writeResult.hasError) {
      if (writeResult.errorMessage) {
        fprintf(stderr, "Output Error: %s\n", writeResult.errorMessage);
      }
      freeResult(&parseResult);
      freeResult(&inputResult);
      freeResult(&processResult);
      freeResult(&writeResult);
      return 1;
    }
    freeResult(&writeResult);
  } else {
    // Print result to stdout
    if (options->decode) {
      printf("Decoded: %s\n", (char *)processResult.data);
    } else {
      printf("Encoded: %s\n", (char *)processResult.data);
    }
  }

  // Cleanup
  freeResult(&parseResult);
  freeResult(&inputResult);
  freeResult(&processResult);

  return 0;
}

/* Parse command line arguments using getopt_long - ReqNonFunc05 */
static Result parseCommandLine(int argc, char **argv) {
  Options *options = malloc(sizeof(Options));
  if (!options) {
    return createError(MORSE_MEMORY_ERROR, "Memory allocation failed");
  }

  // Initialize options
  *options =
      (Options){false, false, false, false, false, NULL, NULL, NULL, false};

  // Define long options
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"programmer-info", no_argument, 0, 'p'},
      {"encode", no_argument, 0, 'e'},
      {"decode", no_argument, 0, 'd'},
      {"out", required_argument, 0, 'o'},
      {"slash-wordspacer", no_argument, 0, 's'}, // ReqOptFunc02
      {0, 0, 0, 0}};

  int option_index = 0;
  int c;

  while ((c = getopt_long(argc, argv, "hpedo:", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 'h':
      options->help = true;
      return createSuccess(options);
    case 'p':
      options->programmerInfo = true;
      return createSuccess(options);
    case 'e':
      options->encode = true;
      break;
    case 'd':
      options->decode = true;
      break;
    case 'o':
      options->outputFile = strdup_safe(optarg);
      break;
    case 's':
      options->slashWordspacer = true;
      break;
    case '?':
      free(options);
      return createError(MORSE_INVALID_OPTION, "Invalid option");
    default:
      free(options);
      return createError(MORSE_INVALID_OPTION, "Unknown option");
    }
  }

  // ReqFunc07: Check for conflicting options
  if (options->encode && options->decode) {
    free(options);
    return createError(
        MORSE_CONFLICTING_OPTIONS,
        "Cannot specify both encode (-e) and decode (-d) options");
  }

  // ReqFunc05: Default is encode if neither specified
  if (!options->decode && !options->encode) {
    options->encode = true;
  }

  // Handle remaining arguments (input text or file)
  if (optind < argc) {
    // Check if it's a readable file or treat as input text
    if (access(argv[optind], R_OK) == 0) {
      options->inputFile = strdup_safe(argv[optind]);
    } else {
      options->inputText = strdup_safe(argv[optind]);
    }
  }

  // ReqFunc10: Check if input is from pipe
  options->readFromPipe = isInputFromPipe();

  return createSuccess(options);
}

/* Display help information - ReqFunc01, ReqFunc02 */
static void displayHelp(void) {
  printf("Morse Code Converter - Help\n");
  printf("============================\n\n");
  printf("A functional CLI application to convert text to Morse code and vice "
         "versa.\n\n");
  printf("USAGE:\n");
  printf("  morse [OPTIONS] [INPUT_TEXT|INPUT_FILE]\n\n");
  printf("OPTIONS:\n");
  printf("  -h, --help                 Display this help message\n");
  printf("  -e, --encode               Encode text to Morse code (default if "
         "not specified)\n");
  printf("  -d, --decode               Decode Morse code to text\n");
  printf("  -o, --out FILE             Write output to specified file instead "
         "of stdout\n");
  printf(
      "  --slash-wordspacer         Use ' / ' between words (encode only)\n");
  printf("  --programmer-info          Display information about the "
         "programmer\n\n");
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
         "output.txt\n");
  printf("  morse -e --slash-wordspacer \"HELLO WORLD\"     Use slash word "
         "separator\n\n");
  printf("SUPPORTED CHARACTERS:\n");
  printf("  - Letters: A-Z (case insensitive)\n");
  printf("  - Numbers: 0-9\n");
  printf("  - Symbols: Space, ., ,, :, ;, ?, !, =, -, +, _, (, ), /, @\n\n");
}

/* Display programmer information in JSON format - ReqFunc03 */
static void displayProgrammerInfo(void) {
  printf("{\n");
  printf("  \"name\": \"Diego Rubio Carrera\",\n");
  printf("  \"program\": \"TIK\",\n");
  printf("  \"email\": \"diegorubiocarrera@gmail.com\"\n");
  printf("}\n");
}

/* Check if input is coming from a pipe */
static bool isInputFromPipe(void) { return !isatty(STDIN_FILENO); }

/* Read content from stdin */
static Result readFromStdin(void) {
  const size_t CHUNK_SIZE = 1024;
  size_t capacity = CHUNK_SIZE;
  size_t size = 0;
  char *buffer = malloc(capacity);

  if (!buffer) {
    return createError(MORSE_MEMORY_ERROR, "Memory allocation failed");
  }

  char chunk[CHUNK_SIZE];
  size_t bytesRead;

  while ((bytesRead = fread(chunk, 1, CHUNK_SIZE - 1, stdin)) > 0) {
    if (size + bytesRead + 1 > capacity) {
      capacity *= 2;
      char *newBuffer = realloc(buffer, capacity);
      if (!newBuffer) {
        free(buffer);
        return createError(MORSE_MEMORY_ERROR, "Memory reallocation failed");
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

  return createSuccess(buffer);
}

/* Read file content - ReqFunc08, ReqFunc09, ReqOptFunc01 */
static Result readFileContent(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    char errorMsg[512];
    snprintf(errorMsg, sizeof(errorMsg), "Could not open file '%s'", filename);
    return createError(MORSE_FILE_NOT_FOUND, errorMsg);
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (fileSize < 0) {
    fclose(file);
    return createError(MORSE_FILE_READ_ERROR, "Could not determine file size");
  }

  // Allocate buffer
  char *buffer = malloc(fileSize + 1);
  if (!buffer) {
    fclose(file);
    return createError(MORSE_MEMORY_ERROR, "Memory allocation failed");
  }

  // Read file content
  size_t bytesRead = fread(buffer, 1, fileSize, file);
  fclose(file);

  if (bytesRead < (size_t)fileSize) {
    free(buffer);
    return createError(MORSE_FILE_READ_ERROR, "Could not read complete file");
  }

  buffer[bytesRead] = '\0';

  // Trim trailing newline if present
  if (bytesRead > 0 && buffer[bytesRead - 1] == '\n') {
    buffer[bytesRead - 1] = '\0';
  }

  return createSuccess(buffer);
}

/* Write content to file - ReqFunc11, ReqFunc12 */
static Result writeFileContent(const char *filename, const char *content) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    char errorMsg[512];
    snprintf(errorMsg, sizeof(errorMsg), "Could not open file '%s' for writing",
             filename);
    return createError(MORSE_FILE_WRITE_ERROR, errorMsg);
  }

  size_t contentLength = strlen(content);
  size_t bytesWritten = fwrite(content, 1, contentLength, file);
  fclose(file);

  if (bytesWritten != contentLength) {
    return createError(MORSE_FILE_WRITE_ERROR,
                       "Could not write complete content to file");
  }

  return createSuccess(NULL);
}

/* Safe string duplication */
static char *strdup_safe(const char *str) {
  if (!str)
    return NULL;

  char *duplicate = strdup(str);
  if (!duplicate) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  return duplicate;
}

/* Get Morse code for a character */
static const char *getCharacterCode(char c) {
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
static char getCodeCharacter(const char *code) {
  const int tableSize = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

  for (int i = 0; i < tableSize; i++) {
    if (strcmp(MORSE_TABLE[i].code, code) == 0) {
      return MORSE_TABLE[i].character;
    }
  }
  return '\0'; // Code not found
}

/* Encode text to Morse code - ReqFunc13-21, ReqFunc23, ReqFunc25-28,
 * ReqOptFunc02 */
static Result encodeText(const char *text, bool useSlashWordspacer) {
  if (!text) {
    return createSuccess(strdup_safe(""));
  }

  // Calculate required buffer size
  size_t maxBufferSize = strlen(text) * 10;
  char *result = malloc(maxBufferSize);
  if (!result) {
    return createError(MORSE_MEMORY_ERROR, "Memory allocation failed");
  }
  result[0] = '\0';

  bool lastWasSpace = false;
  bool firstChar = true;

  for (size_t i = 0; text[i] != '\0'; i++) {
    char currentChar = text[i];

    // ReqFunc28: Skip newlines and carriage returns
    if (currentChar == '\n' || currentChar == '\r') {
      continue;
    }

    // Handle word separation
    if (currentChar == ' ') {
      if (!lastWasSpace && !firstChar) {
        if (useSlashWordspacer) {
          // ReqOptFunc02: Use " / " between words
          strcat(result, " / ");
        } else {
          // ReqFunc27: Use triple space between words
          strcat(result, "   ");
        }
      }
      lastWasSpace = true;
      continue;
    }

    // ReqFunc26: Add space between letters (except before first letter)
    if (!firstChar && !lastWasSpace) {
      strcat(result, " ");
    }

    const char *code = getCharacterCode(currentChar);
    if (code) {
      strcat(result, code);
    } else {
      // ReqFunc25: Output * for unsupported characters
      strcat(result, "*");
    }

    lastWasSpace = false;
    if (firstChar) {
      firstChar = false;
    }
  }

  return createSuccess(result);
}

/* Decode Morse code to text - ReqFunc14, ReqFunc16, ReqFunc18, ReqFunc20,
 * ReqFunc22, ReqFunc24 */
static Result decodeText(const char *morse) {
  if (!morse) {
    return createSuccess(strdup_safe(""));
  }

  size_t maxOutputSize = strlen(morse) + 1;
  char *result = malloc(maxOutputSize);
  if (!result) {
    return createError(MORSE_MEMORY_ERROR, "Memory allocation failed");
  }
  result[0] = '\0';

  char codeBuf[20] = {0};
  size_t codeBufIndex = 0;
  size_t resultIndex = 0;
  int spaceCount = 0;

  for (size_t i = 0; morse[i] != '\0'; i++) {
    // ReqFunc28: Skip newlines and carriage returns
    if (morse[i] == '\n' || morse[i] == '\r') {
      continue;
    }

    if (morse[i] == ' ') {
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
        spaceCount = 0;
      }
    } else if (morse[i] == '/') {
      // Handle slash word separator
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
      // Part of a morse character
      spaceCount = 0;
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

  return createSuccess(result);
}
