"""
Name: LOGAN VAUGHAN
ID #:  1577489
CMPUT  274  EA1

Exercise  4: Deck of Cards
"""

import sys


class Deck:
    def __init__(self, cards):
    	# this will create a copy of the inputed list of cards

    	self.__deck = list(cards)

    def deal(self):
    	# deal(self) will use the pop() function to remove the first card in the list self.__deck
    	# which is the equivalent of taking the top card off of a deck. deal(self) will return
    	# this card or, if the deck (self.__deck) is empty, it will return the value False

    	if len(self.__deck) > 0:
    		draw = self.__deck.pop(0)
    	else: 
    		draw = False

    	return draw

    def validate(self):
    	# validate(self) will make sure a deck contains the right cards. It returns a tuple with
    	# the first value being a true or false statement (true if the deck is valid, false if not)
    	# and the second value being a string describing why the deck is invalid

    	validity = True
    	message = ""

    	if len(self.__deck) > 52:
    		message = "Incomplete Deck"
    		validity = False

    	else:
    		dict = {}
    		count = 1
    		i = 0
    		while i in range(len(self.__deck)):
    			if not(self.__deck[i][0] in "23456789TJQKA") or not(self.__deck[i][1] in "SHDC"):
    				message = "Card " + str(count) + " is not a valid card"
    				validity = False
    				i = len(self.__deck)
    			elif self.__deck[i] in dict:
    				message = "Deck contains duplicate cards"
    				validity = False
    				i = len(self.__deck)
    			else:
    				dict[self.__deck[i]] = 1
    				count += 1
    				i += 1

    	return (validity, message)

    def __str__(self):
    	return '-'.join(self.__deck)


def make_deck(deckfile):
	# takes the input deckfile which is a file containing a deck of cards where every line in the
	# file has the abbreviation of a card name. This function will put the cards of this file into
	# a list and return the list

	fin = open(deckfile, 'r')

	cardlist = []
	for line in fin:
		cardname = line.strip().upper()
		cardlist.append(cardname)

	fin.close()

	return cardlist


def pregame_check(play, statement):
	# ends the program and displays an error statement if play is false (deck is invalid)
	
	if not(play):
		print(statement)
		sys.exit()


def play_highcarddraw():
	# plays high card draw until end of the deck and displays the winner of each round

	x = 1
	while x > 0:
		opponentcard = my_deck.deal()
		usercard = my_deck.deal()
		if not(opponentcard) or not(usercard):
			x = 0
		else:
			if opponentcard[0] > usercard[0]:
				print("Round {}: Opponent wins!".format(x))
			elif opponentcard[0] < usercard[0]:
				print("Round {}: Player wins!".format(x))
			elif opponentcard[0] == usercard[0]:
				print("Round {}: Tie!".format(x))
			x += 1

   
if __name__ == "__main__":

    new_deck = make_deck(sys.argv[1])

    my_deck = Deck(new_deck)

    (valid, reason) = my_deck.validate()

    pregame_check(valid, reason)

    play_highcarddraw()