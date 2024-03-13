# Team_Unicode
Developed during Fall 2021 Semester

UNO Final Project

Description of the game it models: 
The game that we selected is UNO, in which the primary objective is to get rid of your cards faster than any other player in the game. To begin, each player is dealt 7 cards, which include a mixture of regular cards and special cards. The regular cards have 1 of 4 colors(Red,Green,Yellow,Blue) and 1 of 10 numbers(0-9). Within the special cards, there are those with a predetermined color(Reverse, Skip, and Draw 2) and those that have no color associated with them to begin(Wild, Wild Draw 4). According to the true rules of uno, each of the special cards does as follows:

Reverse - Switches the order of the gameplay(ie player 1 then player 2 then player 3 plays -  CHANGES TO - player 3 then player 2 then player 1 plays)

Skip - Passes the next player in the round for one turn(ie player 1 then player 3 plays)

Draw 2 - Employs the same approach as Skip but the player skipped must also draw 2 cards

Wild - Allows the player who plays this card to select what color is next

Wild Draw 4 - Combines the functions of Wild, Skip and Draw 2 by allowing the player who plays this card to select the next color, then forcing the following player to be skipped while also drawing 4 cards. 

In terms of actual game play, the players move around the circle in order(or reverse order when applicable) playing cards that match the last played card either in color or number(When applicable). 

Changes to the game: 
As mentioned above, all of the special cards are treated the same way they would normally in a game; however, the 1 difference in our game comes in checking the player/(s) hand for UNO. While they would normally have until the next player plays a card to say UNO or would otherwise have to draw 2 cards, we instead decided that they need to enter Y/N after each of their turns to signify whether they are on their last card. If they fail to enter Y when they do in fact only have 1 card, then they are forced to draw 2 cards.

Interesting Features: 
Although many play the game with the option to stack the effects of Draw 2, Skip, and Wild Draw 4 cards, the true rules are clear that these feature is not allowed. Therefore, whenever a player plays any of the aforementioned cards, the following player MUST accept the applicable penalty and cannot play the same card to double its effects for the next player.

With regards to playing a card, each card in the player's hand is displayed in the format [Card X: Color Number]. To play the card, enter the number 'X' followed by the Enter button.

Disclaimers: 
*If there is no reaction after entering the input, press enter again as the player may have just entered the input too quickly.
*Our implementation is created with a single round of UNO in mind. In order to play another round, the player must run the code again. This is a result of the memory allocation style used.
*The game is set for play by no more than 8 players(AI/Human) total.

How to compile/run code: We chose to run our code on CLion using C++14 on both windows and mac.

Status Report: All elements of the code are fully functioning with all special cards and features working as expected.
