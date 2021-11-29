// UNO Game created by
// Team Unicode
// Alex Daum, Raviteja Aechan, Jonathan Lanes
// amdaum@ncsu.edu, raechan@ncsu.edu, jmlanes@ncsu.edu,
// ECE 309 Section 001
// running on C++ 14
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm> //<-- used in deck shuffle (shuffling a vector)
#include <stdio.h>   // used for random number generator for AI wild card color selection
#include <stdlib.h>  // ^
#include <time.h>    // ^

using namespace std;

int randomNum(int mod) {
    return (rand() % mod);
}

//overarching game object, encapsulates all relevant game objects/functions to create and run the game.
class UNO_game {
public:

    //card object, represents individual cards that will be passed between the deck and player's hands
    //card object has variables for each card's color/number/type, and member variables to access/set them
    class Card {
    protected:
        int cardColor; // 0 = no color (works with any color), 1 = Red, 2 = Green, 3 = Blue, 4 = Yellow
        int cardNumber; // -1 = skip, -2 = reverse, -3 = draw 2, -4 = wild, -5 = wild draw 4, 0-9 for rest
        char cardType; // R = reg, 2 = +2, 4 = +4W, W = wild, S = skip, X = reverse
    public:
        Card(int col, int num, char type) {
            cardColor = col;
            cardNumber = num;
            cardType = type;
        }

        //returns a card object's color
        int color(){
            return cardColor;
        }

        //returns a card object's number
        int number(){
            return cardNumber;
        }

        //returns a card object's type
        char type(){
            return cardType;
        }

        //translates card object's internal variables to a readable string describing the card
        //used for terminal prompts
        string getName(){
            string color, number, name;
            switch(cardColor){
                case 0:
                    color = "";
                    break;
                case 1:
                    color = "Red ";
                    break;
                case 2:
                    color = "Green ";
                    break;
                case 3:
                    color = "Blue ";
                    break;
                case 4:
                    color = "Yellow ";
                    break;
            }
            switch(cardNumber){
                case -1:
                    number = "Skip";
                    break;
                case -2:
                    number = "Reverse";
                    break;
                case -3:
                    number = "Draw 2";
                    break;
                case -4:
                    number = "Wild";
                    break;
                case -5:
                    number = "Wild Draw 4";
                    break;
                default:
                    number = to_string(cardNumber);
                    break;
            }
            name = color+number;
            return name;
        }

        //sets a card's color
        //used only for Wild/+4 Wild card effect implementation
        void setColor(int i){
            cardColor = i;
        }
    };


    //Deck works as both the draw deck and discard pile for card objects, any cards discarded are pushed to the bottom of the deck
    class Deck {
    protected:
        int drawCount = 30;     //counter used to determine when to shuffle the deck, every 30 cards drawn, deck is shuffled
    public:

        //pointer to card last placed in discard pile
        Card *lastCard;
        queue<Card*> deck;
        Deck(){}

        //draws a card from the deck (also removes from deck)
        //Outputs: Card *retCard : Card pointer to the card that is drawn from deck
        Card* draw(){
            Card *retCard = deck.front();
            deck.pop();
            drawCount++;
            return retCard;
        }

        //adds a card to the bottom of the deck (Discard pile)
        //Inputs: Card *c : pointer to the card that is being discarded
        void discard(Card *c) {
            deck.push(c);
            lastCard = c;
        }

        //shuffle checks drawCount to determine if the "discard" pile needs to be shuffled, and shuffles accordingly
        //pops everything off the deck onto a vector, shuffles it, and adds back to the queue
        void shuffle() {
            if (drawCount >= 30) {
                vector<Card*> unshuffled;
                while (deck.size()!=0) {
                    unshuffled.push_back(deck.front());
                    deck.pop();
                }
                random_shuffle(unshuffled.begin(), unshuffled.end(), randomNum);
                while (!unshuffled.empty()) {
                    deck.push(unshuffled.back());
                    unshuffled.pop_back();
                }
                drawCount = 0;
            }else{}
            return;
        }
    };

    //Player object is an abstract base class used to implement human and ai derived classes
    //contains variables for a player's name, a vector of cards, placeholder variables for derived classes,
    //and member functions to handle player interaction with the game
    class player{
    public:
        string playerName = "<PLAYER NAME>";
        vector<Card*> hand;
        int handCount;
        bool aiStatus;

        //assigns the player's name
        //Input: string name: name that is being assigned to the player
        player(string name){
            playerName = name;
        }

        //checks if a player has an empty card hand
        //Output: Boolean: returns true if player has empty hand, false otherwise
        bool emptyHand(){
            return hand.empty();
        }

        //checks if a player has uno (if their hand has 1 card)
        //Output: Boolean: returns true if player has uno, false otherwise
        bool unoCheck(){
            if(hand.size() == 1)
                return true;
            else
                return false;
        }

        //returns the pointer to the card that is in the player's hand at index i
        //Input: int i: index of card that is being accessed
        //Output: Card* retcard: pointer to the card that is being accessed
        Card* getCard(int i){
            if(i > hand.size()-1 || emptyHand())
                return nullptr;
            Card *retCard = hand.at(i);
            return retCard;
        }

        //returns the pointer to the card that is in the player's hand at index i
        //and removes that card pointer from the player's hand
        //Input: int i: index of card that is being accessed
        //Output: Card* retcard: pointer to the card that is removed from player's hand
        Card* discardCard(int i){
            if(i > hand.size()-1 || emptyHand())
                return nullptr;
            Card *retCard = hand.at(i); //card to be removed is moved to the end of player's hand
            hand.at(i) = hand.at(hand.size()-1);
            hand.pop_back(); //card pointer to be removed is popped from player's hand
            return retCard;
        }

        //adds a card to player's hand
        //Input: Card* n: pointer to the card that is being added to the player's hand
        void addCard(Card* n){
            hand.push_back(n);
        }

        //prints out the hand of a player
        //Output: outputs the cards in a player's hand to the console
        void printHand(){
            cout << "\n";
            for(int i = 0; i < hand.size(); i++){
                cout <<"[Card "<< i+1 << ": "<< getCard(i)->getName()<<"] | ";
            }
            cout << "\n";
        }

        //virtual functions for derived classes
        virtual string playRound() = 0;
        virtual void unoInput(Deck *deck) = 0;
    };

    //humanPlayer object is derived from player base class, contains member functions to handle player input prompts
    //and interaction with the game
    class humanPlayer : public player{
    public:

        //humanPlayer constructor
        //inputs: string name: name of human player
        humanPlayer(string name):player(name){
            playerName = name;
            aiStatus = false;
        }

        //asks player what card they want to play or if they want to draw from deck
        //outputs: string retString: the index to the card that the player has chosen
        string playRound() override{
            string retString;
            bool valid = false;
            cout << playerName << ", it's your turn! Here are your cards:\n";
            printHand();
            cout << "\nTo play a card, select it's number, to draw a card, enter \"d\"\n";
            cin >> retString;
            while(!valid) { //run in a loop until the player has chosen a card that is in the deck or has chosen to draw
                if (retString != "d" && !((stoi(retString) - 1 ) <= hand.size() - 1 && stoi(retString) - 1 >= 0)) { //fix valid input check
                    cout << "\nInvalid Input - Choose another card\nHere are your cards:\n";
                    printHand();
                    cin >> retString;
                }
                else {
                    valid = true;
                }
            }
            return retString;
        }

        //asks if the player has uno, and checks to see if they answered correctly
        //If they had uno and guessed incorrectly, two cards are drawn into their hand
        //Input: Deck* deck: pointer that points to the deck of cards
        //Output: outputs whether or not the player has Uno to the output
        void unoInput(Deck *deck) override{
            char playerInput;
            cout << "\nDo you have UNO? Type 'y' for yes and 'n' for no.\n";
            cin >> playerInput;
            if(playerInput == 'y' && unoCheck()){
                cout << "\n" << this->playerName << " has UNO!\n";
                return;
            }
            else if(playerInput == 'n' && !unoCheck()){
                return;
            }
            else if(playerInput == 'y' && !unoCheck()){
                cout << "\n" << this->playerName << " does NOT have UNO.\n";
            }
            else{
                cout << "\nWRONG! "<< this->playerName << " has UNO, draw two cards.\n";
                this->addCard(deck->draw()); //2 cards are drawn to the player's hand
                this->addCard(deck->draw());
            }
        }
    };

    //aiPlayer object is derived from player base class, contains member functions for computer players' game logic
    class aiPlayer : public player{
    public:
        //aiPlayer constructor
        //inputs: string name: name of ai player
        aiPlayer(string name):player(name){
            playerName = name;
            aiStatus = true; //sets ai identifier to true
            handCount = 1;
        }

        //"computer" player chooses a card to play by looping through their hand until they choose a valid card
        //otherwise they draw a card from deck
        //output: string: returns the index of the card that ai player has player or 'd' if the ai player has drawn a card
        string playRound() override{
            if(handCount > hand.size()){
                handCount = 1;
                return "d";
            }
            else
                return to_string(handCount);
        }

        //if empty hand , draw card else loop until valid card
        //Input: Deck* deck: pointer that points to the deck of cards
        //output: outputs to console if the aiplayer has uno
        void unoInput(Deck *deck) override{
            if(unoCheck())
                cout << "\n" << playerName << " has UNO!\n";
            return;
        }
    };

    //pointers/containers for game operation, upon calling gameStart() will point to an initialized deck, a wildCard placeholder card
    //as well as a vector of pointers to initialized players
    Deck *cardDeck;
    vector<player*> players;
    Card *wildCard;

    int roundCounter = 0;           //keeps track of current game round
    bool reverseStatus = false;     //keeps track of playing order

    //initializes all 108 cards of the game within the deck object
    //Input: Deck *d: pointer to Deck object which will hold card objects
    void deckInit(Deck *d){
        for(int j=1; j<5;j++) {  //loops through colors
            Card *temp = new Card(j, 0, 'R');
            d->deck.push(temp);                         //add '0' card
            temp = new Card(j, -1, 'S');
            d->deck.push(temp);                        //two skip cards for each color
            temp = new Card(j, -1, 'S');
            d->deck.push(temp);
            temp = new Card(j, -2, 'X'); //two reverse cards for each color
            d->deck.push(temp);
            temp = new Card(j, -2, 'X');
            d->deck.push(temp);
            temp = new Card(j, -3, '2'); //two draw 2 cards for each color
            d->deck.push(temp);
            temp = new Card(j, -3, '2');
            d->deck.push(temp);
            for (int i = 1; i < 10; i++){ //adds 1-9 cards for each color
                temp = new Card(j, i, 'R');
                d->deck.push(temp);
                temp = new Card(j, i, 'R');
                d->deck.push(temp);
            }
        }
        for(int i=0;i<4;i++){                              //adds 4 wild and 4 draw 4 cards to deck
            Card *temp = new Card(0, -4, 'W');
            d->deck.push(temp);
            temp = new Card(0, -5, '4');
            d->deck.push(temp);
        }
    }

    //handles initialization of all objects within the game, automatically creates wildCard placeholder card and deck object,
    //calls deckInit and then shuffles the deck of cards, also allows the player to create up to 8 human/ai players for the game
    void gameStart(){
        wildCard = new Card(0, -4, 'W');
        cardDeck = new Deck(); //creates new deck
        deckInit(cardDeck);    //creates/allocates space for all 108 cards
        cardDeck->shuffle();   // shuffles deck
        char menu = 'y';
        string name;
        int type;
        int AICount = 1;
        while(menu=='y' && (players.size() <= 7)){
            cout<<"Would you like to add a human player or AI?\nPress 0 for human and 1 for AI:\n";
            cin>>type;
            if(type==0){ //runs if user wants to add human player
                cout<<"Enter name:\n";
                cin>>name;
                player *newPlayer = new humanPlayer(name);
                players.push_back(newPlayer);
            }else if(type==1){
                string concatName = "aiPlayer" + to_string(AICount);
                player *newPlayer = new aiPlayer(concatName);
                players.push_back(newPlayer); // Push back aiPLayer# to players vector
                AICount++;
            }
            cardDeck->lastCard = cardDeck->deck.back(); //will be the first card of the game

            if(players.size() <= 7) {
                cout << "Would you like to add another player? \nPress 'y' if you want to add another player or 'n' if not\n";
                cin >> menu;
            }
        }

        for(int i = 0; i<players.size();i++){   //distributes 7 cards to all players at start of game
            for(int j=0; j<7;j++){
                players[i]->addCard(cardDeck->draw());
            }
        }
    }

    //compares whatever card the player has chosen to the last card played, determines if the card was a valid choice
    //Input: Card* playedCard: pointer to card object chosen by the player
    //Output: boolean determining whether chosen card was valid or not
    bool compareCard(Card* playedCard){
        //separate results depending on if player is AI or not
        if(players.at(roundCounter)->aiStatus == false) { //runs if current player is human player
            if (playedCard->number() == cardDeck->lastCard->number())
                return true;
            else if (playedCard->color() == cardDeck->lastCard->color())
                return true;
            else if (playedCard->color() == 0)
                return true;
            else                                        //asks human player to chose another card if they chose an invalid card
                cout << "\nInvalid Card, Please choose another card\nLast Card Played: "
                     << cardDeck->lastCard->getName() << "\n";
            return false;
        }
        else{                                           //runs if current player is ai player
            if (playedCard->number() == cardDeck->lastCard->number()) { //outputs to console if aiplayer has chosen a valid card
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else if (playedCard->color() == cardDeck->lastCard->color()) {//outputs to console if aiplayer has chosen a valid card
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else if (playedCard->color() == 0) {//outputs to console if aiplayer has chosen a valid card
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else {                              //returns false if aiplayer has chosen an invalid card
                players.at(roundCounter)->handCount++;
                return false;
            }
        }
    }

    //increments the round counter according to the reverseStatus variable, if forward - increment counter, if reverse - decrement counter
    //ensures counter is within the bounds of the number of players playing, loops around to continue round order
    void incrementRound(){ //Increments round counter accounting for reverseStatus
        if(reverseStatus){
            roundCounter--;
            if(roundCounter<0){
                roundCounter = players.size()-1;
            }
        }
        else{
            roundCounter++;
            if(roundCounter > players.size()-1){
                roundCounter = 0;
            }
        }
    }

    //handles gameplay effects of "special" cards (skipping turns, playing a wild card, drawing cards, reversing round order)
    //Inputs: Card *card: pointer to card object
    //        Deck *deck: pointer to Deck object from which to pull draw card penalties
    void cardEffect(Card *card, Deck *deck){
        char playerInput;
        bool valid = false;
        switch(card->type()){
            case '2':
                incrementRound();
                players.at(roundCounter)->addCard(deck->draw());
                players.at(roundCounter)->addCard(deck->draw());
                break;
            case '4':
                while(!valid){

                    //1 = Red, 2 = Green, 3 = Blue, 4 = Yellow
                    if(players.at(roundCounter)->aiStatus == false) {
                        cout << "\nWhat color would you like to choose? Enter 'r' for red, 'g' for green, 'b' for blue, or 'y' for yellow.\n";
                        cin >> playerInput;
                        switch (playerInput) {
                            case ('r'):
                                valid = true;
                                wildCard->setColor(1);
                                break;
                            case ('g'):
                                valid = true;
                                wildCard->setColor(2);
                                break;
                            case ('b'):
                                valid = true;
                                wildCard->setColor(3);
                                break;
                            case ('y'):
                                valid = true;
                                wildCard->setColor(4);
                                break;
                        }
                    }
                    else{
                        valid = true;
                        wildCard->setColor(2); //add random chooser
                    }
                }
                deck->lastCard = wildCard;

                incrementRound();
                players.at(roundCounter)->addCard(deck->draw());
                players.at(roundCounter)->addCard(deck->draw());
                players.at(roundCounter)->addCard(deck->draw());
                players.at(roundCounter)->addCard(deck->draw());
                break;
            case 'W':
                while(!valid){
                    if(players.at(roundCounter)->aiStatus == false) {
                        cout << "\nWhat color would you like to choose? Enter 'r' for red, 'g' for green, 'b' for blue, or 'y' for yellow.";
                        cin >> playerInput;

                        switch (playerInput) {
                            case ('r'):
                                valid = true;
                                wildCard->setColor(1);
                                break;
                            case ('g'):
                                valid = true;
                                wildCard->setColor(2);
                                break;
                            case ('b'):
                                valid = true;
                                wildCard->setColor(3);
                                break;
                            case ('y'):
                                valid = true;
                                wildCard->setColor(4);
                                break;
                        }
                    }
                    else{
                        valid = true;
                        wildCard->setColor(4); //add random chooser
                    }
                }
                deck->lastCard = wildCard;
                break;
            case 'S':
                incrementRound();
                break;
            case 'X':
                reverseStatus = !reverseStatus;
                break;
            default: //regular card
                break;
        }
        return;
    }

    //plays 1 round of the game, where the current player chooses a card to play
    //outputs last card played, calls playRound, and does the respective action based on player's input:
        //If the player chose to draw, draws card from deck to player's hand
        //If the player played a card, checks if valid card, removes from player's hand, asks if they have Uno, adds to discard pile, and calls cardEffect on that card
    void gamePlay(){
        string playerInput;
        player *currPlayer = players.at(roundCounter);

        bool valid = false;
        cout << "\n\n\n\nLast Card Played: " << cardDeck->lastCard->getName() <<"\n";

        while(!valid){
            playerInput = currPlayer->playRound();
            if(playerInput=="d"){
                if(players.at(roundCounter)->aiStatus == true) {
                    cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                    cout<< players.at(roundCounter)->playerName <<" drew a card from the deck.";
                }
                currPlayer->addCard(cardDeck->draw());
                valid = true;
            }
            else{
                int playerInputInt = stoi(playerInput);
                if(compareCard(currPlayer->getCard(playerInputInt-1))){
                    Card *tmp = currPlayer->getCard(playerInputInt-1);
                    cardDeck->discard(currPlayer->discardCard(playerInputInt-1));
                    cardEffect(tmp, cardDeck);
                    if(!(currPlayer->emptyHand()))
                        currPlayer->unoInput(cardDeck);
                    valid = true;
                }
            }
        }

        cardDeck->shuffle();
        incrementRound();
    }

    //checks for end condition, returns true if any player has 0 cards in their hand vector
    //Output: boolean determining whether end condition has been met
    bool gameEnd(){
        for(int i = 0; i<players.size();i++){
            if(players[i]->emptyHand()==true){
                cout << "\nCongratulations " << players[i]->playerName << ", you won!\n";
                return true;
            }
        }
        return false;
    }
};

int main() {
    UNO_game *game = new UNO_game;      //initializes UNO_game object
    game->gameStart();                  //initializes internal game objects & players
    while(!game->gameEnd()){            //play next round until a player has won
        game->gamePlay();               //play game round
    }
    return 0;
}