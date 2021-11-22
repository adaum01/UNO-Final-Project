//running on C++ 14
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

class UNO_game {
public:
    class Card {
    protected:
        string ID;
        int cardColor; // 0 = no color (works with any color), 1 = Red, 2 = Green, 3 = Blue, 4 = Yellow
        int cardNumber; // -1 = no number, 0-9 for rest
        char cardType; // R = reg, 2 = +2, 4 = +4W, W = wild, S = skip, X = reverse
    public:
        /**Card(string a , int col, int num, char type) {
            ID = a;
            cardColor = col;
            cardNumber = num;
            cardType = type;
        } **/
        Card(int col, int num, char type) {
            cardColor = col;
            cardNumber = num;
            cardType = type;
        }

        string name() {
            return ID;
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
    };


    //Deck works as both the draw deck and discard pile, any cards discarded are pushed to the bottom of the deck
    /*
     * TO-DO
     * - Constructor
     * - shuffle function - will be called every round, check drawCount to shuffle
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

            return retCard;
        }

        void discard(Card c) {
            deck.push(&c);
            lastCard = &c;
        }
        /**void deckPrint(){ //prints out deck size, used for testing
            cout << deck.size() << "\n";
        }**/


        //shuffle checks drawCount to determine if the "discard" pile needs to be shuffled
        //pops everything off the deck onto a vector, shuffles it, and adds back to the queue
        void shuffle() {
            if (drawCount >= 30) {
                vector<Card*> unshuffled;
               while (deck.size()!=0) { //!= nullptr added when testing with printing
                    unshuffled.push_back(deck.front());
                    /**cout<<deck.front()->color(); //Testing to see if properly printed
                    cout<<deck.front()->number();
                    cout<<deck.front()->type();
                    cout<<"\n";**/
                    deck.pop();
                }
                //cout<<"shuffled:";
                random_shuffle(unshuffled.begin(), unshuffled.end());
                while (!unshuffled.empty()) {
                    deck.push(unshuffled.back());
                    /**cout<<unshuffled.back()->color(); //prints out shuffled deck
                    cout<<unshuffled.back()->number();
                    cout<<unshuffled.back()->type();
                    cout<<"\n";**/
                    unshuffled.pop_back();
                }
                drawCount = 0;
            }else{}
            return;
        }
    };               //Implement shuffle

    /*
     * TO - DO
     * Set player names?
     */
    class player{
    public:
        string playerName = "<PLAYER NAME>";
        vector<Card*> hand;
        bool skipStatus = false;
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
        void addCard(Card* n){
            hand.push_back(n);
        }
        void printHand(){       //DO NOT CALL ON EMPTY HAND
            cout << "\n";
            for(int i = 0; i < hand.size(); i++){
                cout << i+1 << " - "<< getCard(i)->name();
            }
            cout << "\n";
        }

        virtual char playRound() = 0;
    };

    /*
     * TO-DO
     * card choice invalid choices
     */
    class humanPlayer : public player{
    public:
        humanPlayer(string name):player(name){
            playerName = name;
        }
        char playRound() override{
            char retChar;
            cout << playerName << ", your turn! Here are your cards";
            printHand();
            cout << "To play a card, select it's number, to draw a card, enter \"d\"";
            cin >> retChar;
            if(retChar != 'D' || (retChar - '0') > hand.size()-1);{
                cin >> retChar;
            }
            //Check if card is valid for the last card pushed discarded
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
        }
        char playRound() override{
            return 0;
        }
        //if empty hand , draw card else loop until valid card
    };

    Deck *cardDeck;
    vector<player*> players;

    void deckInit(Deck *d){
        for(int j=1; j<5;j++) {  //loops through colors
            Card *temp = new Card(j, 0, 'R');
            d->deck.push(temp);                         //add '0' card
            temp = new Card(j, -1, 'S');
            d->deck.push(temp);                        //two skip cards for each color
            temp = new Card(j, -1, 'S');
            d->deck.push(temp);
            temp = new Card(j, -1, 'X'); //two reverse cards for each color
            d->deck.push(temp);
            temp = new Card(j, -1, 'X');
            d->deck.push(temp);
            temp = new Card(j, -1, '2'); //two draw 2 cards for each color
            d->deck.push(temp);
            temp = new Card(j, -1, '2');
            d->deck.push(temp);
            for (int i = 1; i < 10; i++){ //adds 1-9 cards for each color
                temp = new Card(j, i, 'R');
                d->deck.push(temp);
                temp = new Card(j, i, 'R');
                d->deck.push(temp);
            }

        }
        for(int i=0;i<4;i++){                              //adds 4 wild and 4 draw 4 cards to deck
            Card *temp = new Card(0, -1, 'W');
            d->deck.push(temp);
            temp = new Card(0, -1, '4');
            d->deck.push(temp);
        }

    }
    /*
     * TO-DO
     * AI name assignment,
     * Wrong button checks for "menu loop" and player addition
     */
    void gameStart(){
        cardDeck = new Deck(); //creates new deck and allocates cards
        deckInit(cardDeck);//initializes deck with all 108 cards
       //cardDeck->deckPrint();
        cardDeck->shuffle();         // shuffles deck
        //cardDeck->deckPrint();
        char menu = 'y';
        string name;
        int type;
        int AICount = 1;
        while(menu=='y'){
            cout<<"Would you like to add a human player or AI? Press 0 for human and 1 for AI";
            cin>>type;
            if(type==0){ //runs if user wants to add human player
                cout<<"Enter name:";
                cin>>name;
                player *newPlayer = new humanPlayer(name);
                players.push_back(newPlayer);
            }else if(type==1){
                //string name = "placeholder";
                string concatName = "aiPlayer" + to_string(AICount);
                player *newPlayer = new aiPlayer(concatName);
                players.push_back(newPlayer); // Push back aiPLayer# to players vector
                AICount++;
            }
            cout<<"Would you like to add another player? Press 'y' if you want to add another player or 'n' if not";
            cin>>menu;

        }
        for(int i = 0; i<players.size();i++){
            for(int j=0; j<7;j++){
                players[i]->addCard(cardDeck->draw());
            }
        }
    }
    bool gameEnd(){
        for(int i = 0; i<players.size();i++){
            if(players[i]->emptyHand()==true){
                return true;
            }
        }
        return false;
    }




};

int main() {

    UNO_game *game = new UNO_game;
    game->gameStart();
    cout << "test";

    //discardPile *p = new discardPile;
    //Card *c = new Card;

}

//{Blue_0,Blue_1,Blue_1,Blue_2,Blue_2,Blue_3,Blue_3,Blue_4,Blue_4,Blue_5,Blue_5,Blue_6,Blue_6,Blue_7,Blue_7,Blue_8,Blue_8,Blue_9,Blue_9,Green_0,Green_1,Green_1,Green_2,Green_2,Green_3,Green_3,Green_4,Green_4,Green_5,Green_5,Green_6,Green_6,Green_7,Green_7,Green_8,Green_8,Green_9,Green_9,Red_0,Red_1,Red_1,Red_2,Red_2,Red_3,Red_3,Red_4,Red_4,Red_5,Red_5,Red_6,Red_6,Red_7,Red_7,Red_8,Red_8,Red_9,Red_9,Yellow_0,Yellow_1,Yellow_1,Yellow_2,Yellow_2,Yellow_3,Yellow_3,Yellow_4,Yellow_4,Yellow_5,Yellow_5,Yellow_6,Yellow_6,Yellow_7,Yellow_7,Yellow_8,Yellow_8,Yellow_9,Yellow_9,Yellow_Skip, Yellow_Skip, Blue_Skip, Blue_Skip, Green_Skip, Green_Skip, Red_Skip, Red_Skip, Yellow_Reverse, Yellow_Reverse,Blue_Reverse,Blue_Reverse,Green_Reverse,Green_Reverse,Red_Reverse, Red_Reverse, Yellow_+2, Yellow_+2, Blue_+2, Blue_+2, Green_+2, Green+2, Red_+2, Red_+2, Wild, Wild, Wild, Wild, Wild_+4, Wild_+4, Wild_+4, Wild_+4}