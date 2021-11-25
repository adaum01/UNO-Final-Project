// UNO Game created by
// Team Unicode
// Alex Daum, Raviteja Aechan, Jonathan Lanes
// amdaum@ncsu.edu,raechan@ncsu.edu, jmlanes@ncsu.edu,
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

class UNO_game {
public:
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

        int color(){
            return cardColor;
        }

        int number(){
            return cardNumber;
        }

        char type(){
            return cardType;
        }

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

        void setColor(int i){
            cardColor = i;
        }
    };


    //Deck works as both the draw deck and discard pile, any cards discarded are pushed to the bottom of the deck
    /*
     * TO-DO
     * - Constructor
     */
    class Deck {
    protected:
        int drawCount = 30;
    public:
        //pointer to card last placed in discard pile
        Card *lastCard;

        queue<Card*> deck; //= new queue<Card>;         //create constructor for deck
        Deck(){}

        Card* draw(){
            Card *retCard = deck.front();
            deck.pop();
            drawCount++;
            return retCard;
        }

        void discard(Card *c) {
            deck.push(c);
            lastCard = c;
        }

        //shuffle checks drawCount to determine if the "discard" pile needs to be shuffled
        //pops everything off the deck onto a vector, shuffles it, and adds back to the queue
        void shuffle() {
            if (drawCount >= 30) {
                vector<Card*> unshuffled;
                while (deck.size()!=0) {
                    unshuffled.push_back(deck.front());
                    deck.pop();
                }
                random_shuffle(unshuffled.begin(), unshuffled.end());
                while (!unshuffled.empty()) {
                    deck.push(unshuffled.back());
                    unshuffled.pop_back();
                }
                drawCount = 0;
            }else{}
            return;
        }
    };

    /*
     * TO - DO
     * Set player names?
     */
    class player{
    public:
        string playerName = "<PLAYER NAME>";
        vector<Card*> hand;
        int handCount;
        bool aiStatus;
        player(string name){
            playerName = name;
        }
        bool emptyHand(){
            return hand.empty();
        }
        bool unoCheck(){
            if(hand.size() == 1)
                return true;
            else
                return false;
        }

        Card* getCard(int i){
            if(i > hand.size()-1 || emptyHand())
                return nullptr;
            Card *retCard = hand.at(i);
            return retCard;
        }

        Card* discardCard(int i){
            if(i > hand.size()-1 || emptyHand())
                return nullptr;
            Card *retCard = hand.at(i);
            hand.at(i) = hand.at(hand.size()-1);
            hand.pop_back();
            return retCard;
        }

        void addCard(Card* n){
            hand.push_back(n);
        }
        void printHand(){       //DO NOT CALL ON EMPTY HAND
            cout << "\n";
            for(int i = 0; i < hand.size(); i++){
                cout <<"[Card "<< i+1 << ": "<< getCard(i)->getName()<<"] | ";
            }
            cout << "\n";
        }

        virtual string playRound() = 0;
        virtual void unoInput(Deck *deck) = 0;
    };

    /*
     * TO-DO
     * card choice invalid choices
     */
    class humanPlayer : public player{
    public:
        humanPlayer(string name):player(name){
            playerName = name;
            aiStatus = false;
        }
        string playRound() override{
            string retString;
            bool valid = false;
            cout << playerName << ", it's your turn! Here are your cards:\n";
            printHand();
            cout << "\nTo play a card, select it's number, to draw a card, enter \"d\"\n";
            cin >> retString;
            while(!valid) {
                if (retString != "d" && !((stoi(retString) - 1 ) <= hand.size() - 1 && stoi(retString) - 1 >= 0)) {
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
                this->addCard(deck->draw());
                this->addCard(deck->draw());
            }
        }
    };

    /*
     * TO-DO
     * implement playRound function
     */
    class aiPlayer : public player{
    public:
        aiPlayer(string name):player(name){
            playerName = name;
            aiStatus = true;
            handCount = 1;
        }

        string playRound() override{
            if(handCount > hand.size()){
                handCount = 1;
                return "d";
            }
            else
                return to_string(handCount);
        }
        //if empty hand , draw card else loop until valid card

        void unoInput(Deck *deck) override{
            if(unoCheck())
                cout << "\n" << playerName << " has UNO!\n";
            return;
        }
    };

    Deck *cardDeck;
    vector<player*> players;
    Card *wildCard;

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
    /*
     * TO-DO
     * Wrong button checks for "menu loop" and player addition
     */
    void gameStart(){
        wildCard = new Card(0, -4, 'W');
        cardDeck = new Deck(); //creates new deck and allocates cards
        deckInit(cardDeck);//initializes deck with all 108 cards
        cardDeck->shuffle();         // shuffles deck
        char menu = 'y';
        string name;
        int type;
        int AICount = 1;
        while(menu=='y'){
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
            cardDeck->lastCard = cardDeck->deck.back();

            cout<<"Would you like to add another player? \nPress 'y' if you want to add another player or 'n' if not\n";
            cin>>menu;

        }
        for(int i = 0; i<players.size();i++){
            for(int j=0; j<7;j++){
                players[i]->addCard(cardDeck->draw());
            }
        }
    }

    int roundCounter = 0;
    bool reverseStatus = false;

    bool compareCard(Card* playedCard){
        //separate results depending on if player is AI or not
        if(players.at(roundCounter)->aiStatus == false) {
            if (playedCard->number() == cardDeck->lastCard->number())
                return true;
            else if (playedCard->color() == cardDeck->lastCard->color())
                return true;
            else if (playedCard->color() == 0)
                return true;
            else
                cout << "\nInvalid Card, Please choose another card\nLast Card Played: "
                     << cardDeck->lastCard->getName() << "\n";
            return false;
        }
        else{
            if (playedCard->number() == cardDeck->lastCard->number()) {
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else if (playedCard->color() == cardDeck->lastCard->color()) {
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else if (playedCard->color() == 0) {
                cout << "\n" << players.at(roundCounter)->playerName << "'s turn!\n";
                cout << players.at(roundCounter)->playerName << " played card: " << playedCard->getName();
                players.at(roundCounter)->handCount = 1;
                return true;
            }
            else {
                players.at(roundCounter)->handCount++;
                return false;
            }
        }
    }


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
                    currPlayer->unoInput(cardDeck);
                    valid = true;
                }
            }
        }

        cardDeck->shuffle();
        incrementRound();
    }

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
    UNO_game *game = new UNO_game;
    game->gameStart();
    while(!game->gameEnd()){
        game->gamePlay();
    }
}