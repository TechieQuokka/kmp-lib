Running main() from C:\b\abs_14_rhzemyc\croot\yaml-cpp-split_1714514162675\work\test\gtest-1.11.0\googletest\src\gtest_main.cc
Note: Google Test filter = FileTestRunner.*
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from FileTestRunner
[ RUN      ] FileTestRunner.TestCasesFile

================================================================================
  FILE: tests/data/testcases.txt
  TOTAL TEST CASES: 75
================================================================================

--------------------------------------------------------------------------------
TEST #  1 [Line  10]  [PASSED]  (2 us)
  Text    : "hello world"
  Pattern : "world"
  Expected: 6
  Actual  : 6
--------------------------------------------------------------------------------
TEST #  2 [Line  11]  [PASSED]  (0 us)
  Text    : "The quick brown fox jumps over the lazy dog"
  Pattern : "fox"
  Expected: 16
  Actual  : 16
--------------------------------------------------------------------------------
TEST #  3 [Line  12]  [PASSED]  (0 us)
  Text    : "Programming in C++ is both challenging and rewarding"
  Pattern : "C++"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST #  4 [Line  15]  [PASSED]  (0 us)
  Text    : "The beginning of wisdom is the definition of terms"
  Pattern : "The"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST #  5 [Line  16]  [PASSED]  (0 us)
  Text    : "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  Pattern : "ABC"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST #  6 [Line  17]  [PASSED]  (0 us)
  Text    : "Start of the string matters for performance"
  Pattern : "Start"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST #  7 [Line  20]  [PASSED]  (0 us)
  Text    : "This is the final word"
  Pattern : "word"
  Expected: 18
  Actual  : 18
--------------------------------------------------------------------------------
TEST #  8 [Line  21]  [PASSED]  (0 us)
  Text    : "Looking for the needle in this haystack needle"
  Pattern : "needle"
  Expected: 16,40
  Actual  : 16,40
--------------------------------------------------------------------------------
TEST #  9 [Line  22]  [PASSED]  (0 us)
  Text    : "All roads lead to Rome"
  Pattern : "Rome"
  Expected: 18
  Actual  : 18
--------------------------------------------------------------------------------
TEST # 10 [Line  25]  [PASSED]  (0 us)
  Text    : "The beautiful sunset painted the sky with brilliant colors"
  Pattern : "sky"
  Expected: 33
  Actual  : 33
--------------------------------------------------------------------------------
TEST # 11 [Line  26]  [PASSED]  (0 us)
  Text    : "In the middle of difficulty lies opportunity"
  Pattern : "middle"
  Expected: 7
  Actual  : 7
--------------------------------------------------------------------------------
TEST # 12 [Line  27]  [PASSED]  (0 us)
  Text    : "Between two evils I always pick the one I never tried before"
  Pattern : "evils"
  Expected: 12
  Actual  : 12
--------------------------------------------------------------------------------
TEST # 13 [Line  30]  [PASSED]  (0 us)
  Text    : "abracadabra"
  Pattern : "abra"
  Expected: 0,7
  Actual  : 0,7
--------------------------------------------------------------------------------
TEST # 14 [Line  31]  [PASSED]  (0 us)
  Text    : "Mississippi"
  Pattern : "issi"
  Expected: 1,4
  Actual  : 1,4
--------------------------------------------------------------------------------
TEST # 15 [Line  32]  [PASSED]  (0 us)
  Text    : "banana banana banana"
  Pattern : "banana"
  Expected: 0,7,14
  Actual  : 0,7,14
--------------------------------------------------------------------------------
TEST # 16 [Line  33]  [PASSED]  (0 us)
  Text    : "the the the the"
  Pattern : "the"
  Expected: 0,4,8,12
  Actual  : 0,4,8,12
--------------------------------------------------------------------------------
TEST # 17 [Line  34]  [PASSED]  (1 us)
  Text    : "aaaaaaaaa"
  Pattern : "aaa"
  Expected: 0,1,2,3,4,5,6
  Actual  : 0,1,2,3,4,5,6
--------------------------------------------------------------------------------
TEST # 18 [Line  37]  [PASSED]  (0 us)
  Text    : "hello world"
  Pattern : "xyz"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 19 [Line  38]  [PASSED]  (1 us)
  Text    : "The quick brown fox"
  Pattern : "elephant"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 20 [Line  39]  [PASSED]  (0 us)
  Text    : "ABCDEFG"
  Pattern : "HIJKLMN"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 21 [Line  40]  [PASSED]  (0 us)
  Text    : "completely different strings here"
  Pattern : "notfound"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 22 [Line  47]  [PASSED]  (1 us)
  Text    : "aaaaaaaaaaaaaaaa"
  Pattern : "aa"
  Expected: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
  Actual  : 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
--------------------------------------------------------------------------------
TEST # 23 [Line  48]  [PASSED]  (0 us)
  Text    : "abababababab"
  Pattern : "abab"
  Expected: 0,2,4,6,8
  Actual  : 0,2,4,6,8
--------------------------------------------------------------------------------
TEST # 24 [Line  49]  [PASSED]  (0 us)
  Text    : "aaabaaaabaaaabaaa"
  Pattern : "aab"
  Expected: 1,6,11
  Actual  : 1,6,11
--------------------------------------------------------------------------------
TEST # 25 [Line  50]  [PASSED]  (0 us)
  Text    : "abcabcabcabc"
  Pattern : "abcabc"
  Expected: 0,3,6
  Actual  : 0,3,6
--------------------------------------------------------------------------------
TEST # 26 [Line  51]  [PASSED]  (0 us)
  Text    : "xyxyxyxyxyxy"
  Pattern : "xyxy"
  Expected: 0,2,4,6,8
  Actual  : 0,2,4,6,8
--------------------------------------------------------------------------------
TEST # 27 [Line  54]  [PASSED]  (0 us)
  Text    : "ABCABDABCABC"
  Pattern : "ABCABC"
  Expected: 6
  Actual  : 6
--------------------------------------------------------------------------------
TEST # 28 [Line  55]  [PASSED]  (0 us)
  Text    : "abaababaabaab"
  Pattern : "abaab"
  Expected: 0,5,8
  Actual  : 0,5,8
--------------------------------------------------------------------------------
TEST # 29 [Line  56]  [PASSED]  (0 us)
  Text    : "xyzxyzxyzxyz"
  Pattern : "xyzxyz"
  Expected: 0,3,6
  Actual  : 0,3,6
--------------------------------------------------------------------------------
TEST # 30 [Line  63]  [PASSED]  (0 us)
  Text    : "hello   world"
  Pattern : "   "
  Expected: 5
  Actual  : 5
--------------------------------------------------------------------------------
TEST # 31 [Line  64]  [PASSED]  (0 us)
  Text    : "line1	column2	line2"
  Pattern : "	"
  Expected: 5,13
  Actual  : 5,13
--------------------------------------------------------------------------------
TEST # 32 [Line  67]  [PASSED]  (0 us)
  Text    : "Hello, World! How are you?"
  Pattern : ", "
  Expected: 5
  Actual  : 5
--------------------------------------------------------------------------------
TEST # 33 [Line  68]  [PASSED]  (0 us)
  Text    : "C++ is great; Java is too."
  Pattern : "; "
  Expected: 12
  Actual  : 12
--------------------------------------------------------------------------------
TEST # 34 [Line  69]  [PASSED]  (0 us)
  Text    : "Question? Answer! Statement."
  Pattern : "?"
  Expected: 8
  Actual  : 8
--------------------------------------------------------------------------------
TEST # 35 [Line  72]  [PASSED]  (0 us)
  Text    : "path/to/file.txt"
  Pattern : "/to/"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 36 [Line  73]  [PASSED]  (0 us)
  Text    : "email@domain.com"
  Pattern : "@"
  Expected: 5
  Actual  : 5
--------------------------------------------------------------------------------
TEST # 37 [Line  74]  [PASSED]  (0 us)
  Text    : "price: $99.99"
  Pattern : "$"
  Expected: 7
  Actual  : 7
--------------------------------------------------------------------------------
TEST # 38 [Line  75]  [PASSED]  (0 us)
  Text    : "100% complete!"
  Pattern : "%"
  Expected: 3
  Actual  : 3
--------------------------------------------------------------------------------
TEST # 39 [Line  82]  [PASSED]  (0 us)
  Text    : "Hello World"
  Pattern : "hello"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 40 [Line  83]  [PASSED]  (0 us)
  Text    : "UPPERCASE"
  Pattern : "uppercase"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 41 [Line  84]  [PASSED]  (0 us)
  Text    : "MixedCase"
  Pattern : "mixedcase"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 42 [Line  87]  [PASSED]  (0 us)
  Text    : "HelloHelloHello"
  Pattern : "Hello"
  Expected: 0,5,10
  Actual  : 0,5,10
--------------------------------------------------------------------------------
TEST # 43 [Line  88]  [PASSED]  (0 us)
  Text    : "ABCabcABCabc"
  Pattern : "ABC"
  Expected: 0,6
  Actual  : 0,6
--------------------------------------------------------------------------------
TEST # 44 [Line  89]  [PASSED]  (0 us)
  Text    : "TestTESTtest"
  Pattern : "TEST"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 45 [Line  96]  [PASSED]  (0 us)
  Text    : "1234567890"
  Pattern : "456"
  Expected: 3
  Actual  : 3
--------------------------------------------------------------------------------
TEST # 46 [Line  97]  [PASSED]  (1 us)
  Text    : "9999999999"
  Pattern : "999"
  Expected: 0,1,2,3,4,5,6,7
  Actual  : 0,1,2,3,4,5,6,7
--------------------------------------------------------------------------------
TEST # 47 [Line  98]  [PASSED]  (0 us)
  Text    : "2024-12-19"
  Pattern : "12"
  Expected: 5
  Actual  : 5
--------------------------------------------------------------------------------
TEST # 48 [Line  99]  [PASSED]  (0 us)
  Text    : "123.456.789"
  Pattern : "456"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 49 [Line 102]  [PASSED]  (0 us)
  Text    : "version2.0.1"
  Pattern : "2.0"
  Expected: 7
  Actual  : 7
--------------------------------------------------------------------------------
TEST # 50 [Line 103]  [PASSED]  (0 us)
  Text    : "user123admin"
  Pattern : "123"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 51 [Line 104]  [PASSED]  (0 us)
  Text    : "data2024backup"
  Pattern : "2024"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 52 [Line 105]  [PASSED]  (0 us)
  Text    : "file001file002file003"
  Pattern : "file"
  Expected: 0,7,14
  Actual  : 0,7,14
--------------------------------------------------------------------------------
TEST # 53 [Line 112]  [PASSED]  (0 us)
  Text    : "aaaaaaa"
  Pattern : "a"
  Expected: 0,1,2,3,4,5,6
  Actual  : 0,1,2,3,4,5,6
--------------------------------------------------------------------------------
TEST # 54 [Line 113]  [PASSED]  (0 us)
  Text    : "abcdefg"
  Pattern : "d"
  Expected: 3
  Actual  : 3
--------------------------------------------------------------------------------
TEST # 55 [Line 114]  [PASSED]  (0 us)
  Text    : "xyzxyzxyz"
  Pattern : "z"
  Expected: 2,5,8
  Actual  : 2,5,8
--------------------------------------------------------------------------------
TEST # 56 [Line 117]  [PASSED]  (0 us)
  Text    : "a"
  Pattern : "a"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 57 [Line 118]  [PASSED]  (0 us)
  Text    : "x"
  Pattern : "y"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 58 [Line 121]  [PASSED]  (0 us)
  Text    : "exactly"
  Pattern : "exactly"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 59 [Line 122]  [PASSED]  (0 us)
  Text    : "match"
  Pattern : "match"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 60 [Line 125]  [PASSED]  (0 us)
  Text    : "hi"
  Pattern : "hello"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 61 [Line 126]  [PASSED]  (0 us)
  Text    : "ab"
  Pattern : "abcd"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 62 [Line 129]  [PASSED]  (0 us)
  Text    : "abcdef"
  Pattern : "abcdfg"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 63 [Line 130]  [PASSED]  (0 us)
  Text    : "hello world"
  Pattern : "hello_world"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 64 [Line 131]  [PASSED]  (0 us)
  Text    : "close match"
  Pattern : "close_match"
  Expected: NOT_FOUND
  Actual  : NOT_FOUND
--------------------------------------------------------------------------------
TEST # 65 [Line 138]  [PASSED]  (0 us)
  Text    : "[ERROR] Connection failed [ERROR] Retry attempt"
  Pattern : "[ERROR]"
  Expected: 0,26
  Actual  : 0,26
--------------------------------------------------------------------------------
TEST # 66 [Line 141]  [PASSED]  (0 us)
  Text    : "function getData() { } function setData() { }"
  Pattern : "function"
  Expected: 0,23
  Actual  : 0,23
--------------------------------------------------------------------------------
TEST # 67 [Line 144]  [PASSED]  (0 us)
  Text    : "<div><div>Hello</div></div>"
  Pattern : "<div"
  Expected: 0,5
  Actual  : 0,5
--------------------------------------------------------------------------------
TEST # 68 [Line 151]  [PASSED]  (0 us)
  Text    : "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaneedle"
  Pattern : "needle"
  Expected: 50
  Actual  : 50
--------------------------------------------------------------------------------
TEST # 69 [Line 154]  [PASSED]  (0 us)
  Text    : "The quick brown fox"
  Pattern : "quick brown"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 70 [Line 161]  [PASSED]  (0 us)
  Text    : "getUserDataFromDatabaseConnection"
  Pattern : "Data"
  Expected: 7,15
  Actual  : 7,15
--------------------------------------------------------------------------------
TEST # 71 [Line 162]  [PASSED]  (0 us)
  Text    : "calculateTotalPriceWithDiscount"
  Pattern : "Price"
  Expected: 14
  Actual  : 14
--------------------------------------------------------------------------------
TEST # 72 [Line 163]  [PASSED]  (0 us)
  Text    : "processInputValidationResult"
  Pattern : "Validation"
  Expected: 12
  Actual  : 12
--------------------------------------------------------------------------------
TEST # 73 [Line 170]  [PASSED]  (0 us)
  Text    : "the cat and the dog and the bird"
  Pattern : "the"
  Expected: 0,12,24
  Actual  : 0,12,24
--------------------------------------------------------------------------------
TEST # 74 [Line 171]  [PASSED]  (0 us)
  Text    : "hello hello hello"
  Pattern : "hello"
  Expected: 0,6,12
  Actual  : 0,6,12
--------------------------------------------------------------------------------
TEST # 75 [Line 174]  [PASSED]  (0 us)
  Text    : "This is a test. This is a test."
  Pattern : "This is a test"
  Expected: 0,16
  Actual  : 0,16

================================================================================
  SUMMARY: tests/data/testcases.txt
================================================================================
  Total    : 75 test cases
  Passed   : 75
  Failed   : 0
  Pass Rate: 100.0%
  Time     : 0 ms
================================================================================

[       OK ] FileTestRunner.TestCasesFile (0 ms)
[ RUN      ] FileTestRunner.LongSentencesFile

================================================================================
  FILE: tests/data/long_sentences.txt
  TOTAL TEST CASES: 50
================================================================================

--------------------------------------------------------------------------------
TEST #  1 [Line   9]  [PASSED]  (1 us)
  Text    : "The fundamental principles of quantum mechanics describe the..."
  Pattern : "quantum"
  Expected: 30
  Actual  : 30
--------------------------------------------------------------------------------
TEST #  2 [Line  10]  [PASSED]  (0 us)
  Text    : "The process of photosynthesis in plants involves the convers..."
  Pattern : "photosynthesis"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST #  3 [Line  11]  [PASSED]  (0 us)
  Text    : "In the field of artificial intelligence and machine learning..."
  Pattern : "neural networks"
  Expected: 67
  Actual  : 67
--------------------------------------------------------------------------------
TEST #  4 [Line  12]  [PASSED]  (0 us)
  Text    : "The human genome consists of approximately three billion bas..."
  Pattern : "genome"
  Expected: 10
  Actual  : 10
--------------------------------------------------------------------------------
TEST #  5 [Line  13]  [PASSED]  (0 us)
  Text    : "Climate change represents one of the most significant challe..."
  Pattern : "Climate change"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST #  6 [Line  19]  [PASSED]  (0 us)
  Text    : "The development of blockchain technology has introduced a ne..."
  Pattern : "blockchain"
  Expected: 19
  Actual  : 19
--------------------------------------------------------------------------------
TEST #  7 [Line  20]  [PASSED]  (0 us)
  Text    : "Modern software development practices emphasize continuous i..."
  Pattern : "continuous integration"
  Expected: 48
  Actual  : 48
--------------------------------------------------------------------------------
TEST #  8 [Line  21]  [PASSED]  (0 us)
  Text    : "The architecture of microservices has transformed how large-..."
  Pattern : "microservices"
  Expected: 20
  Actual  : 20
--------------------------------------------------------------------------------
TEST #  9 [Line  22]  [PASSED]  (0 us)
  Text    : "Cloud computing platforms provide on-demand access to comput..."
  Pattern : "Cloud computing"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 10 [Line  23]  [PASSED]  (0 us)
  Text    : "The emergence of quantum computing threatens to break many c..."
  Pattern : "quantum computing"
  Expected: 17
  Actual  : 17
--------------------------------------------------------------------------------
TEST # 11 [Line  29]  [PASSED]  (0 us)
  Text    : "In the ancient library of Alexandria, scholars studied manus..."
  Pattern : "library of Alexandria"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST # 12 [Line  30]  [PASSED]  (0 us)
  Text    : "The detective carefully examined the crime scene, noting eve..."
  Pattern : "detective"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 13 [Line  31]  [PASSED]  (0 us)
  Text    : "As the sun set over the vast African savanna, the pride of l..."
  Pattern : "African savanna"
  Expected: 29
  Actual  : 29
--------------------------------------------------------------------------------
TEST # 14 [Line  32]  [PASSED]  (0 us)
  Text    : "The old lighthouse keeper had spent forty years maintaining ..."
  Pattern : "lighthouse"
  Expected: 8
  Actual  : 8
--------------------------------------------------------------------------------
TEST # 15 [Line  33]  [PASSED]  (0 us)
  Text    : "Through the dense canopy of the rainforest, sunlight barely ..."
  Pattern : "rainforest"
  Expected: 32
  Actual  : 32
--------------------------------------------------------------------------------
TEST # 16 [Line  39]  [PASSED]  (0 us)
  Text    : "The Renaissance period in European history marked a profound..."
  Pattern : "Renaissance"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 17 [Line  40]  [PASSED]  (1 us)
  Text    : "The Industrial Revolution fundamentally changed economic and..."
  Pattern : "Industrial Revolution"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 18 [Line  41]  [PASSED]  (0 us)
  Text    : "The ancient civilization of Mesopotamia developed between th..."
  Pattern : "Mesopotamia"
  Expected: 28
  Actual  : 28
--------------------------------------------------------------------------------
TEST # 19 [Line  42]  [PASSED]  (0 us)
  Text    : "The Silk Road connected the great civilizations of East and ..."
  Pattern : "Silk Road"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 20 [Line  43]  [PASSED]  (0 us)
  Text    : "The French Revolution of 1789 transformed political thought ..."
  Pattern : "French Revolution"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 21 [Line  49]  [PASSED]  (0 us)
  Text    : "The global financial crisis of 2008 exposed weaknesses in th..."
  Pattern : "financial crisis"
  Expected: 11
  Actual  : 11
--------------------------------------------------------------------------------
TEST # 22 [Line  50]  [PASSED]  (0 us)
  Text    : "Sustainable business practices have become increasingly impo..."
  Pattern : "Sustainable business"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 23 [Line  51]  [PASSED]  (0 us)
  Text    : "The gig economy has transformed traditional employment relat..."
  Pattern : "gig economy"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 24 [Line  52]  [PASSED]  (0 us)
  Text    : "Supply chain management has become increasingly complex in t..."
  Pattern : "Supply chain"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 25 [Line  53]  [PASSED]  (0 us)
  Text    : "The cryptocurrency market has experienced extreme volatility..."
  Pattern : "cryptocurrency"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 26 [Line  59]  [PASSED]  (0 us)
  Text    : "The development of vaccines represents one of the greatest a..."
  Pattern : "vaccines"
  Expected: 19
  Actual  : 19
--------------------------------------------------------------------------------
TEST # 27 [Line  60]  [PASSED]  (0 us)
  Text    : "Mental health awareness has grown significantly in recent ye..."
  Pattern : "Mental health"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 28 [Line  61]  [PASSED]  (0 us)
  Text    : "The human microbiome plays a crucial role in digestion and i..."
  Pattern : "microbiome"
  Expected: 10
  Actual  : 10
--------------------------------------------------------------------------------
TEST # 29 [Line  62]  [PASSED]  (0 us)
  Text    : "Precision medicine aims to tailor medical treatment to indiv..."
  Pattern : "Precision medicine"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 30 [Line  63]  [PASSED]  (0 us)
  Text    : "The opioid epidemic has caused devastating harm to communiti..."
  Pattern : "opioid epidemic"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 31 [Line  69]  [PASSED]  (0 us)
  Text    : "The Amazon rainforest contains an estimated ten percent of a..."
  Pattern : "Amazon rainforest"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 32 [Line  70]  [PASSED]  (0 us)
  Text    : "Ocean acidification is threatening marine ecosystems worldwi..."
  Pattern : "Ocean acidification"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 33 [Line  71]  [PASSED]  (0 us)
  Text    : "Biodiversity loss has accelerated dramatically in recent dec..."
  Pattern : "Biodiversity"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 34 [Line  72]  [PASSED]  (0 us)
  Text    : "Renewable energy sources are becoming increasingly cost-comp..."
  Pattern : "Renewable energy"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 35 [Line  73]  [PASSED]  (0 us)
  Text    : "The Great Barrier Reef has experienced severe bleaching even..."
  Pattern : "Great Barrier Reef"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 36 [Line  79]  [PASSED]  (0 us)
  Text    : "The trolley problem is a famous thought experiment in ethics..."
  Pattern : "trolley problem"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 37 [Line  80]  [PASSED]  (0 us)
  Text    : "Existentialist philosophy emphasizes individual freedom and ..."
  Pattern : "Existentialist"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 38 [Line  81]  [PASSED]  (0 us)
  Text    : "The concept of artificial general intelligence raises profou..."
  Pattern : "artificial general intelligence"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST # 39 [Line  82]  [PASSED]  (0 us)
  Text    : "Utilitarianism judges actions by their consequences for over..."
  Pattern : "Utilitarianism"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 40 [Line  83]  [PASSED]  (0 us)
  Text    : "The debate between free will and determinism has engaged phi..."
  Pattern : "free will"
  Expected: 19
  Actual  : 19
--------------------------------------------------------------------------------
TEST # 41 [Line  89]  [PASSED]  (0 us)
  Text    : "The James Webb Space Telescope has provided unprecedented vi..."
  Pattern : "James Webb Space Telescope"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 42 [Line  90]  [PASSED]  (0 us)
  Text    : "Black holes are regions where gravity is so strong that noth..."
  Pattern : "Black holes"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 43 [Line  91]  [PASSED]  (0 us)
  Text    : "The search for extraterrestrial intelligence has expanded be..."
  Pattern : "extraterrestrial intelligence"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST # 44 [Line  92]  [PASSED]  (0 us)
  Text    : "Mars exploration has intensified with multiple rovers studyi..."
  Pattern : "Mars exploration"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 45 [Line  93]  [PASSED]  (0 us)
  Text    : "The Big Bang theory describes the origin of the universe."
  Pattern : "Big Bang"
  Expected: 4
  Actual  : 4
--------------------------------------------------------------------------------
TEST # 46 [Line  99]  [PASSED]  (0 us)
  Text    : "Cognitive behavioral therapy has proven effective for treati..."
  Pattern : "Cognitive behavioral therapy"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 47 [Line 100]  [PASSED]  (0 us)
  Text    : "The psychology of decision-making reveals that humans are su..."
  Pattern : "decision-making"
  Expected: 18
  Actual  : 18
--------------------------------------------------------------------------------
TEST # 48 [Line 101]  [PASSED]  (0 us)
  Text    : "Emotional intelligence is crucial for leadership effectivene..."
  Pattern : "Emotional intelligence"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 49 [Line 102]  [PASSED]  (1 us)
  Text    : "The study of neuroplasticity has revolutionized our understa..."
  Pattern : "neuroplasticity"
  Expected: 13
  Actual  : 13
--------------------------------------------------------------------------------
TEST # 50 [Line 103]  [PASSED]  (0 us)
  Text    : "Attachment theory describes how early relationships shape ou..."
  Pattern : "Attachment theory"
  Expected: 0
  Actual  : 0

================================================================================
  SUMMARY: tests/data/long_sentences.txt
================================================================================
  Total    : 50 test cases
  Passed   : 50
  Failed   : 0
  Pass Rate: 100.0%
  Time     : 0 ms
================================================================================

[       OK ] FileTestRunner.LongSentencesFile (0 ms)
[ RUN      ] FileTestRunner.StressPatternsFile

================================================================================
  FILE: tests/data/stress_patterns.txt
  TOTAL TEST CASES: 19
================================================================================

--------------------------------------------------------------------------------
TEST #  1 [Line  10]  [PASSED]  (1 us)
  Text    : "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..."
  Pattern : "aaaaab"
  Expected: 94
  Actual  : 94
--------------------------------------------------------------------------------
TEST #  2 [Line  13]  [PASSED]  (1 us)
  Text    : "abababababababababababababababab"
  Pattern : "abab"
  Expected: 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28
  Actual  : 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28
--------------------------------------------------------------------------------
TEST #  3 [Line  20]  [PASSED]  (2 us)
  Text    : "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..."
  Pattern : "aaaaaaaaaa"
  Expected: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90
  Actual  : 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90
--------------------------------------------------------------------------------
TEST #  4 [Line  27]  [PASSED]  (0 us)
  Text    : "The quick brown fox and the quick brown fox"
  Pattern : "quick brown"
  Expected: 4,28
  Actual  : 4,28
--------------------------------------------------------------------------------
TEST #  5 [Line  30]  [PASSED]  (0 us)
  Text    : "xyzxyzxyzxyzxyzxyzxyz"
  Pattern : "xyzxyz"
  Expected: 0,3,6,9,12,15
  Actual  : 0,3,6,9,12,15
--------------------------------------------------------------------------------
TEST #  6 [Line  37]  [PASSED]  (1 us)
  Text    : "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  Pattern : "aa"
  Expected: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48
  Actual  : 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48
--------------------------------------------------------------------------------
TEST #  7 [Line  40]  [PASSED]  (1 us)
  Text    : "abababababababababababababababababababababababababab"
  Pattern : "ab"
  Expected: 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50
  Actual  : 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50
--------------------------------------------------------------------------------
TEST #  8 [Line  43]  [PASSED]  (1 us)
  Text    : "abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc"
  Pattern : "abc"
  Expected: 0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51
  Actual  : 0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51
--------------------------------------------------------------------------------
TEST #  9 [Line  50]  [PASSED]  (0 us)
  Text    : "STARTMARKER followed by content"
  Pattern : "STARTMARKER"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 10 [Line  53]  [PASSED]  (0 us)
  Text    : "Content before ENDMARKER"
  Pattern : "ENDMARKER"
  Expected: 15
  Actual  : 15
--------------------------------------------------------------------------------
TEST # 11 [Line  56]  [PASSED]  (0 us)
  Text    : "ExactMatchTest"
  Pattern : "ExactMatchTest"
  Expected: 0
  Actual  : 0
--------------------------------------------------------------------------------
TEST # 12 [Line  63]  [PASSED]  (2 us)
  Text    : "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  Pattern : "!!!!!"
  Expected: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42
  Actual  : 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42
--------------------------------------------------------------------------------
TEST # 13 [Line  70]  [PASSED]  (1 us)
  Text    : "ERROR Connection failed ERROR Retry"
  Pattern : "ERROR"
  Expected: 0,24
  Actual  : 0,24
--------------------------------------------------------------------------------
TEST # 14 [Line  77]  [PASSED]  (0 us)
  Text    : "function one() { } function two() { }"
  Pattern : "function"
  Expected: 0,19
  Actual  : 0,19
--------------------------------------------------------------------------------
TEST # 15 [Line  80]  [PASSED]  (0 us)
  Text    : "const a = 1; const b = 2; const c = 3;"
  Pattern : "const"
  Expected: 0,13,26
  Actual  : 0,13,26
--------------------------------------------------------------------------------
TEST # 16 [Line  87]  [PASSED]  (0 us)
  Text    : "the cat the dog the bird"
  Pattern : "the"
  Expected: 0,8,16
  Actual  : 0,8,16
--------------------------------------------------------------------------------
TEST # 17 [Line  90]  [PASSED]  (0 us)
  Text    : "Test one. Test two. Test three."
  Pattern : "Test"
  Expected: 0,10,20
  Actual  : 0,10,20
--------------------------------------------------------------------------------
TEST # 18 [Line  97]  [PASSED]  (0 us)
  Text    : "abcabcabcabcabcabcabcabcabc"
  Pattern : "abcabcabc"
  Expected: 0,3,6,9,12,15,18
  Actual  : 0,3,6,9,12,15,18
--------------------------------------------------------------------------------
TEST # 19 [Line 100]  [PASSED]  (0 us)
  Text    : "abaababaaba"
  Pattern : "abaab"
  Expected: 0,5
  Actual  : 0,5

================================================================================
  SUMMARY: tests/data/stress_patterns.txt
================================================================================
  Total    : 19 test cases
  Passed   : 19
  Failed   : 0
  Pass Rate: 100.0%
  Time     : 0 ms
================================================================================

[       OK ] FileTestRunner.StressPatternsFile (0 ms)
[----------] 3 tests from FileTestRunner (1 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (2 ms total)
[  PASSED  ] 3 tests.
