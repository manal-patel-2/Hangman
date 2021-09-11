//#ifndef HANGMAN
//#define HANGMAN
#pragma once

using namespace std;

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <string.h>
#include <vector>
#include <algorithm>
#include <ctype.h>

// extern size_t wordLength; // word length specified by the user ( 3 - 12 characters )
// extern int numOfGuesses; // Number of guesses the player would like to have ( x > 0 )
// extern int curtainRevealed; // Whether the player would like to see what the "dodging looks like". Boolean Value.
// extern char* inputFile;

extern void runGame( int argc, char *argv[] );

void initGame();
size_t getWordLength();
int getNumOfGuesses();
int revealCurtain();

void printInfo();

extern int originalGuesses; // Original number of guesses wanted by the user
extern size_t wordLength;
extern int numOfGuesses;
extern int curtainRevealed;
extern char *inputFile; // filename inputed at command line

extern int numOfFamilies;


extern vector<string> dictionary; // List of words obtained from inputFile
extern map<char,int> alphabet; // Container filled with possible letters to use for the game
extern vector<char> printedWord; // Word that is printed to the screen during each round

//string mysteryWord;

extern char playerGuess;

// Sets up the container filled with the mysteryWord
// This will probably not be in the final program
vector<char> setMysteryWord();

// prints the container containing the mystery word
// This will probably not be used in the final program
void printMysteryWord( vector<char> wrd );

// Determines if there is a word that is wordLength long inside of inputFile
int containsWord( char *inputFile, size_t wLen );

// Creates a word list in the form of a vector<string> using the inputFile
vector<string> createListFromIF( char *inFile, size_t wLen );

// Creates a container to hold the alphabet to be used by the player
map<char,int> setCharAlph();
void printUsedChars( map<char,int> alph );

// Ask the user to input their guess for a letter in the word
char promptUser();
extern int isValid;

// Returns a partitioned dictionary
multimap<int,string> partitionByLocOfLetter( char pGuess, multimap<int,string> dict );

extern multimap<int,string> groupedDictionary;

// Converts the initial vector to multimap
multimap<int,string> convertVectorToMultimap( vector<string> orgDict);

int findLocOfLetter( char pGuess, string word );

// Concatenates all the integers in in Locs that are locations
// of the matching letter.
int concatenateIntegers( vector<int> locs );

//  Obtains each distict word family number and returns it in a vector
vector<int> wordFamilies( multimap<int,string> groupDict );

int findMostCommonFamily( multimap<int,string> groupDict, char playerGuess );

// Removes all other families that are not in charLocWithMost
multimap<int,string> removeOtherFamilies( int charLocWithMost, multimap<int,string> groupDict );

// Compares two different families of equal length to determine which is better.
int compareFamilies( int curFamily, int mostCommonFamily, char playerGuess, multimap<int,string> groupDict );

void revealLetters( int locationOfLetters, vector<int> locs );

vector<int> findLocations( char pGuess, string word );

extern vector<int> locsOfMultiple;
// vector of locations of matching letter if word contains more than one

// Determines if the player has revealed all of the letters or not
int wordFound();

extern char replayGame;
extern string userYesOrNo;


int containsWord( char *inputFile, int wordLength );



//#endif
