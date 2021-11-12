//running on C++ 14
#include <iostream>
#include <string>
//#include <vector>
#include <queue>
using namespace std;

    class Card {
    protected:
        string ID;
        int cardColor; // 0 = no color, 1 = Red, 2 = Green, 3 = Blue, 4 = Yellow
        int cardNumber; // -1 = no number, 0-9 for rest
    public:
        Card(string a, int col, int num) {
            ID = a;
            cardColor = col;
            cardNumber = num;
        }

        string cardName() {
            return ID;
        }

        virtual bool playCard(discardPile p) = 0;
    };

    class NormalCard : public Card {
        bool playCard(discardPile p) {
            Card
            if (p.peek())
        }
    };

    class discardPile {
    public:
        queue<Card> *pile = new queue<Card>;

    };

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;

    discardPile *p = new discardPile;
    Card *c = new Card;

}


