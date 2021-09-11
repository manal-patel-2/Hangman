#include "functions.h"
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <string.h>
#include <vector>
#include <algorithm>
#include <ctype.h>

using namespace std;

size_t wordLength;
int originalGuesses; // Original number of guesses wanted by the user
int numOfGuesses;
int curtainRevealed;
char *inputFile; // filename inputed at command line

int numOfFamilies;


vector<string> dictionary; // List of words obtained from inputFile
map<char,int> alphabet; // Container filled with possible letters to use for the game
vector<char> printedWord; // Word that is printed to the screen during each round

// vector of locations of matching letter if \word contains more than one
vector<int> locsOfMultiple;

char playerGuess;

int isValid;

multimap<int,string> groupedDictionary;

char replayGame;
string userYesOrNo;

/**
 * Runs the main game loop as many times as necessary.
 *
 *
 * @param argc number of arguements passed in on command line
 * @param argv char** containing all command line arguements
 */
void runGame( int argc, char *argv[] )
{
	int mostCommonFamilyLoc;
//	cout << "argc" << argc << endl;

	if( argc < 2 || ( argv[1] == NULL || strlen( argv[1] ) == 0 ) )
	{
		cout << "Default file used will be dictionary.txt" << endl;
		inputFile = ( char * ) malloc( strlen( "dictionary.txt" ) );
		strcpy( inputFile, "dictionary.txt" );
	}
	else
	{
		inputFile = ( char * ) malloc( strlen( argv[1] ) );
		inputFile = argv[1];
	}

	initGame();
	groupedDictionary = convertVectorToMultimap( dictionary );

	do
	{
		replayGame = 0;
		do
		{
			printInfo();
			groupedDictionary = partitionByLocOfLetter( playerGuess, groupedDictionary );
			mostCommonFamilyLoc = findMostCommonFamily( groupedDictionary, playerGuess );
			groupedDictionary = removeOtherFamilies( mostCommonFamilyLoc, groupedDictionary );

			if( mostCommonFamilyLoc == 0 )
			{
				numOfGuesses -= 1;
				cout << "Sorry, there are no " << playerGuess << "'s." << endl;
			}
			else
			{
				revealLetters( mostCommonFamilyLoc, locsOfMultiple );
			}
			if( curtainRevealed )
			{
				cerr << "DEBUGGING: words left : " << groupedDictionary.size() << endl;
			}


		} while( ( numOfGuesses != 0 && numOfGuesses != originalGuesses - 10  ) && !wordFound() );

		if( numOfGuesses == 0 || numOfGuesses == originalGuesses - 10 )
		{
			cout << "\n You Lose! The word was: " << groupedDictionary.begin()->second << "." << endl;
		}
		else if( wordFound() )
		{
			cout << "\nAnswer = ";
			printMysteryWord( printedWord );
			cout << "Congratulations!\nYou beat me." << endl;
		}

		cout << "\nPlease enter y for yes, and anything else for no.";
		cout << "\nWould you like to play Again?";
		cin >> userYesOrNo;

		if( userYesOrNo.length() == 1 )
		{
			if( userYesOrNo.at( 0 ) == 'y' )
			{
				replayGame = 1;
				if( replayGame == 1 )
				{
					system( "clear" );
				}
				initGame();
				groupedDictionary = convertVectorToMultimap( dictionary );
			}
		}

	} while( replayGame );

	cout << "\n\nAlright, I guess you don't want to play with me anymore. \"crying in the corner\"";
	cout << "....\nJust because you are leaving, I am just gonna leave all that trash above.\n\n\n";



}

/**
 * Initializes the game by obtaining information
 * from the player.
 *
 *
 */
void initGame()
{
	alphabet = setCharAlph();
    wordLength = getWordLength();
    numOfGuesses = getNumOfGuesses();
	originalGuesses = numOfGuesses;
    curtainRevealed = revealCurtain();
	printedWord = setMysteryWord();

	dictionary = createListFromIF( inputFile, wordLength );

} // end of initGame


/**
 * Obtains the word length from the user, and
 * returns it.
 *
 * @return int - the wanted length of the word.
 */
size_t getWordLength()
{
	int containsLength;
	size_t wLen;
	do
	{
		cout << "\nEnter a the length of the word to be used: ";
		cin >> wLen;

		if( ( wLen < 3 || wLen > 12 ) )
		{
			cout << "\nIncorrect Length\nLength has to be between 3-12" << endl;
		}
		else
		{
			// Used to determine if a word length of wLen is inside of the inputFile
			containsLength = containsWord( inputFile, wLen );

			if( !containsLength )
			{
				cout << "\nIncorrect Length\nWord with Requested length " << wLen <<
					" not found in file provided" << endl;
			}
		}
	} while( ( wLen < 3 || wLen > 12 ) || !containsLength );


	return wLen;
}

/**
 * Obtains the number of guesses the player would like.
 *
 *
 *
 * @return int - number of guesses wanted by player */
int getNumOfGuesses()
{
	int guessNum;
	do
	{
		cout << "\n\nEnter the amount of guesses you would like: ";
		cin >> guessNum;

		if( guessNum <= 0 )
		{
			cout << "\nIncorrect Input.\nNumber of guesses has to be greater than 0" << endl;

		}

	} while( guessNum <= 0 );

	return guessNum;
}

/**
 * Determines if the user would like to see nuts and
 * bolts of the game.
 *
 *
 * @return int - returns 1 if yes, and 0 if otherwise
 */
int revealCurtain()
{
	string userResponse;

	do
	{
		cout << "\n\nWould you like to see the way this world works? ";
		cin >> userResponse;

		if( userResponse.compare( "yes" ) != 0 && userResponse.compare( "no" ) != 0 )
		{
			cout << "\nIncorrect Input.\nPlease enter a lower case yes or no." << endl;
		}

	} while( userResponse.compare( "yes" ) != 0 && userResponse.compare( "no" ) != 0 );

//	cout << " \n\n User Response: " << userResponse << endl;

	// User wants to see everything in the background
	if( userResponse.compare( "yes" ) == 0 )
	{
		return 1;
	}
	// User does not want to see the background stuff

	return 0;

}


/**
 * Returns true if the a word in the dictionary is the same
 * length as wanted by the user.
 *
 * @param inputFile the name of the file used on the command line
 * @param wordLength the length of the word wanted by the user
 * @return int true if they list contains a word with length = wordLength
 */
int containsWord( char *inputFile, size_t wLen )
{
    string word;
    int contains; // does the dictionary contain a word of proper length
    contains = 0;

	ifstream is; // is = input stream
	is.open( inputFile );

	while( getline(is, word) )
	{
		if( word.length() == wLen )
		{
			contains = 1;
		}
	}
    is.close();


	return contains;
}


/**
 * Creates a list from the input file and returns that
 * list.
3 *
 *
 *
 * @param inFile input file that with the list of words
 * @param wordLength the length of the words wanted for list
 * @return vector - vector contianing the each distinct word found in inputFile
 */
vector<string> createListFromIF( char *inFile, size_t wLen )
{
	vector<string> listOfWords;
	ifstream is( inFile ); // is = input stream
	string word;

	if( strcmp( inFile,"dictionary.txt" ) == 0 )
	{
		wLen = wLen + 1;
	}
	while( getline( is, word ) )
	{
		if( word.length() == wLen )
		{
			if( ( count( listOfWords.begin(), listOfWords.end(), word ) == 0 ) )
			{
				listOfWords.push_back( word );
			}

		}

	}
	if( strcmp( inFile,"dictionary.txt" ) == 0 )
	{
        wLen = wLen - 1;
    }

	is.close();

	return listOfWords;

}

/**
 * Prints info necessary for each turn during the game.
 *
 *
 *
 *
 *
 */
void printInfo()
{
	do
	{
		cout << "\nYou have " << numOfGuesses << " guesses left." << endl;
		cout << "Used letters: ";
		printUsedChars( alphabet );
		cout << "Word: ";
		printMysteryWord( printedWord );
		cout << "Enter guess: ";
		playerGuess = promptUser();

	} while( !isValid );

}


/**
 * Prints the characters that the player has already used.
 *
 *
 *
 * @param alph the alphabet used to determine what letters have been used
 */
void printUsedChars( map<char,int> alph )
{

	map<char,int>::iterator it = alph.begin();

	while( it != alph.end() )
	{
		if( it->second == 0 )
		{
			cout << '_' << ' '; // prints out a _ meaning the character has not been used
		}
		else
		{
			cout << it->first << ' '; // prints out the character used
		}
		it++;
	}

	cout << endl;



}


/**
 * Sets up an alphabet container to used as a tracker to know what
 * letters the player has used.
 *
 *
 *
 * @return map<char,int> returns a container filled with characters to be used for game
 */
map<char,int> setCharAlph()
{
	map<char,int> alph;
	int characterVal;
	int counter;

	characterVal = ( int ) 'a';
	counter = 0;

	do
	{
		// Value for map is set to 0 because this is used to allow the print
		// method to know what is a _ and what is a letter used.
		// The program changes the value for a specific key ( or letter ) to
		// make program understand that the letter has been used.
		alph.insert( pair<char,int>( ( char ) characterVal, 0 ) );
		characterVal += 1;

		counter += 1;
	} while( counter < 26 );

	return alph;
}

/**
 * Sets up the container that will hold the information
 * used to represent the mysteryWord that will be printed
 * to the user during each turn.
 *
 * @param wLen the length of the mysteryWord
 * @return multimap<char,int> the container with info about what chars have been revealed
 */
vector<char> setMysteryWord()
{
	vector<char> word;

	size_t counter;
	counter = 0;
	do
	{
		word.push_back( '-' );
		counter++;
	} while( counter != wordLength );

	return word;
}

/**
 * Prints the mystery word and all of the letters found so far.
 *
 *
 *
 * @param wrd container filled with the letters of the mystery word
 */
void printMysteryWord( vector<char> wrd )
{
	size_t it = 0;

	do
	{
		cout << wrd.at(it);
		it++;
	} while( it != wordLength );

	cout << endl;
}

/**
 * Prompt the user to guess a single letter that is
 * part of the mystery word.
 *
 *
 * @return char - the character guessed by the user
 */
char promptUser()
{
	char guess; // the character to be used as the user's guess
	string userIn; // What the user typed
	map<char,int>::iterator it;

	isValid = 0;
	cin >> userIn;
	guess = userIn.at( 0 );
	if( isupper( guess ) )
	{
		guess = guess + 32;
	}
	it = alphabet.find( guess );
	if( userIn.length() == 1 )
	{
		if( isalpha( guess ) )
		{
			if( it->second == 0 )
			{
				isValid = 1; // Allow it to escape infinite loop.

				// change the value at the specific letter to indicate that it has been used
				alphabet[it->first] = 1;
			}
			else
			{
				cout << "\nInvalid Letter. " << userIn.at( 0 ) << " has already been used." << endl;
			}
		}
		else
		{
			cout << "\nInvalid Character. " << userIn.at( 0 ) << " is not in the alphabet." << endl;
		}
	}
	else
	{
		cout << "\nInvalid length.\nGuess a single letter from the alphabet." << endl;
	}

	return guess;
} // end of promptUser


/**
 * Returns a partioned dictionary where the value contains the word
 * and the key is the location of the matching letter. If there
 * is more than one loctaion for the matching letter, the locations are
 * concatenated.
 *
 *
 * @param pGuess the character used by the player
 * @param dict the dictionary containing the current list of remaining words
 *
 * @return multimap<int,string> the partioned directory
 */
multimap<int,string> partitionByLocOfLetter( char pGuess, multimap<int,string> dict )
{
	multimap<int,string> partedDictionary;
	multimap<int,string>::iterator it;
	it = dict.begin();

	while( it != dict.end() )
	{
		partedDictionary.insert( pair<int,string>( findLocOfLetter( playerGuess, it->second ), it->second ) );
		it++;
	}

	return partedDictionary;

}

/**
 * Returns a multimap<string,int> converted from a vector<string>.
 *
 *
 *
 * @param orgDict original dictionary containing the list of words
 * @return multimap<string,int> container filled with the words from orgDict
 *
 */
multimap<int,string> convertVectorToMultimap( vector<string> orgDict )
{
	multimap<int,string> convertedMultimap;

	vector<string>::iterator it;
	it = orgDict.begin();

	while( it != orgDict.end() )
	{
		convertedMultimap.insert( pair<int,string>( 0, *it ) );
		it++;
	}

	return convertedMultimap;

}

/**
 * Finds the location of the specific pGuess inside
 * of word.
 *
 * @param pGuess the character guessed by the player
 * @param word the word to be searched for pGuess
 * @return int the location of pGuess inside of word, 0 if no pGuess in word
 */
int findLocOfLetter( char pGuess, string word )
{
	int location;
	int numOfLetters;

	location = word.find( pGuess );

	if( location == ( int ) string::npos )
	{
		return 0;
	}

	numOfLetters = count( word.begin(), word.end(), pGuess );

	if( numOfLetters > 1 )
	{

		locsOfMultiple = findLocations( pGuess, word );
		location = concatenateIntegers( locsOfMultiple );
	}
	else
	{
		location += 1;
	}

	return location;
}

/**
 * Finds the locations of the matching letter in the word
 * provided
 *
 * @param pGuess the character the player guessed
 * @param word the word that contains instances of the letter
 * @return vector<int> - vector is filled with each location of the letter
 */
vector<int> findLocations( char pGuess, string word )
{
	vector<int> locs;
	int i;


	for( i = 0; i < ( int ) word.length(); i++ )
	{
		if( word.at( i ) == pGuess )
		{
			locs.push_back( i + 1 );
		}
	}

	return locs;


}
/**
 * Returns the concatenation of base and addition.
 *
 *
 *
 * @param locs a vector filled with different locations of the matching letter
 * @return int the concatination of base and addition
 */
int concatenateIntegers( vector<int> locs )
{
	string endString;
    string preString;
	string sufString;
	int numVersion;
	vector<int>::iterator it;
	if( !locs.empty() )
	{
		it = locs.begin();
		while( it != locs.end() )
		{

			if( it != locs.begin() )
			{
				sufString = to_string( *it );
				endString = preString + sufString;
				preString = endString;
			}
			else
			{
				preString = to_string( *it );
			}
			it++;
		}
		numVersion = stoi( endString );

	}
	return numVersion;
}


/**
 * Finds the word family with the most members.
 *
 *
 * @param groupDict dictionary that has already been partitioned into families
 * @param playerGuess the character the player guessed
 * @return int the value for the family with the most members
 *
 */
int findMostCommonFamily( multimap<int,string> groupDict, char playerGuess )
{
	vector<int> distinctLocs; // vector full of distinct char locations
	int charLocOfMostGroup; // the matching char location of the group with most words
	charLocOfMostGroup = 0;
	int numInMostGroup; // the number of words in the family with most words
	numInMostGroup = 0;
	int tempNumMost; // used to compare if there is a bigger family


	distinctLocs = wordFamilies( groupDict );
	vector<int>::iterator it;
	it = distinctLocs.begin();

	while( it != distinctLocs.end() )
	{
		tempNumMost = groupDict.count( *it );
		// This only accounts for if there is one family with the most amount of words
		// Still need to implement if there are multiple families with the same amount of words
		// The deciding factor will be whichever family reveals the least amount of letters
		if( tempNumMost > numInMostGroup )
		{
			numInMostGroup = tempNumMost;
			charLocOfMostGroup = *it;
		}
		else if( tempNumMost == numInMostGroup )
		{
			// *it is the exact current charLoc
			int curLoc;
			curLoc = *it;

			charLocOfMostGroup = compareFamilies( curLoc, charLocOfMostGroup, playerGuess, groupDict );
		}
		it++;
	}



	return charLocOfMostGroup;
}

/**
 * Counts the number of word families, and records
 * the locations of the matching characters
 *
 * @param groupedDict contains the words and the locations of the matching chars
 * @return vector<int> - vector containing the charLocs of the families
 *
 */
vector<int> wordFamilies( multimap<int,string> groupDict )
{
	vector<int> familyLocs;
	int tempCharLoc;
	tempCharLoc = -1;
	multimap<int,string>::iterator it;
	it = groupDict.begin();


	do
	{
		tempCharLoc = it->first;
		if( familyLocs.empty() )
		{
			familyLocs.push_back(tempCharLoc);
		}
		else
		{
			if( find(familyLocs.begin(), familyLocs.end(), tempCharLoc) == familyLocs.end() )
			{
				familyLocs.push_back(tempCharLoc);

			} // end of find if
		} // end of empty() if

		it++;
	}
	while( it != groupDict.end() );

	return familyLocs;
}

/**
 * Removes all other families besides the one that have the matching letter at
 * charLocWithMost.
 *
 * @param charLocWithMost the location of the matching character in the most common family
 * @param groupDict dictionary partitioned into different word families
 * @return multimap<int,string> - dictionary containing only the most common family.
 */
multimap<int,string> removeOtherFamilies( int charLocWithMost, multimap<int,string> groupDict )
{
	multimap<int,string>::iterator it;
	it = groupDict.begin();

	while( it != groupDict.end() )
	{
		if( it->first != charLocWithMost )
		{
			groupDict.erase( it );
		}
		it++;

	}

	return groupDict;
}

/**
 * Determines the number of matching 'letter' in the
 * fullWord.
 *
 * @param letter the letter to find in fullWord
 * @param fullWord word that might contain letter
 * @return int - number of matching letter in fullWord
 */
int countNumOfMatch( char letter, string fullWord )
{
	int counter;
	counter = 0;

	int numOfLetters;
	numOfLetters = 0;

	while( counter < ( int ) fullWord.length() )
	{
		if( fullWord.at(counter) == letter )
		{
			numOfLetters += 1;
		}

		counter += 1;
	}

	return numOfLetters;

}

/**
 * Determines which family contains less of the matching letter between curFamily and
 * mostCommonFamily.
 *
 *
 * @param curFamily the current charLoc of the iterator
 * @param mostCommonFamily the current family with the most members
 * @param playerGuess the character the player guessed
 * @param groupDict the partitioned dictionary
 * @return int - the better family between two families with equal members
 */
int compareFamilies( int curFamily, int mostCommonFamily, char playerGuess, multimap<int,string> groupDict )
{
	int betterFamily;

	int numInCur;
	int numInMost;
	multimap<int,string>::iterator it;

	it = groupDict.find( curFamily );

	numInCur = count( it->second.begin(), it->second.end(), playerGuess );


	it = groupDict.find( mostCommonFamily );

	numInMost = count( it->second.begin(), it->second.end(), playerGuess );


	if( numInCur < numInMost )
	{
		betterFamily = curFamily;
	}
	else if( numInMost < numInCur )
	{
		betterFamily = mostCommonFamily;
	}
	else if( numInMost == numInCur )
	{
		if( curFamily < mostCommonFamily  )
		{
			betterFamily = curFamily;
		}
		else
		{
			betterFamily = mostCommonFamily;
		}
	}

	return betterFamily;
}


/**
 * If there are any matching letters in the most common
 * family, then this will reveal the letters to the user.
 *
 * @param locationOfLetters the location of the letter/letters to be revealed
 * @param locs the locations of the letters to be revealed
 */
void revealLetters( int locationOfLetters, vector<int> locs )
{
	if( locationOfLetters < 11 )
	{
		printedWord.at( locationOfLetters - 1 ) = playerGuess;

		cout << "Yes, there is 1 copy of an " << playerGuess << "." << endl;
	}
	else
	{
		vector<int>::iterator it;
		it = locs.begin();
		while( it != locs.end() )
		{
			printedWord.at( *it - 1 ) = playerGuess;
			it++;
		}
		cout << "Yes, there are " << locs.size() << " copies of " << playerGuess << "."<< endl;

	}
}


/**
 * Determines if the player has revealed all letters in the word.
 *
 *
 * @return int 0 if the word is not found, 1 otherwise
 *
 */
int wordFound()
{
	int gameWon;
	gameWon = 0;

	vector<char>::iterator it;
	it = find( printedWord.begin(), printedWord.end(), '-' );

	if( it == printedWord.end() )
	{
		gameWon = 1;
	}


	return gameWon;
}
