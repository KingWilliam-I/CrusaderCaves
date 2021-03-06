// CrusaderCaves.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "pch.h"
#include <iostream>
#include <vector>
#include "Player.h"
#include "Room.h"
#include "Inventory.h"
#include "Weaponry.h"
#include "Ouch.h"
#include "HereticPawn.h"
#include "Baphomet_Boss.h"

/*
		-~={TO DO}=~-
	[*] = done
	[*] Implement functionality to use item command
		[] Test it and make necessary changes
	[*] rework item contexts to store them in the rooms not the items
	[*] add item contexts
	[] parse and edit room descriptions to reflect player interactions
	[] flesh out enemy classes
	[] add search command
	[] add miracles
	[] add exp yielding process
	[] add story strings
	[] add characters and interactions
	[] fix all the social/economic problems in low income areas
		so that it really is all good in the hood
*/

Player *Crusader = new Player(); // create ya boi
std::vector<Person*> floop; // local, short term storage for room enemies
Baphomet_Boss *Baphomet = new Baphomet_Boss();

// note to self: cap the skill point allocation at 30 for now

inline void PlayerTurn()//Compartmentalizing turns. I'm taking this opportunity to play with inline
{
	std::string reply;
	std::string keyword;
	std::string itemName;

	while (true)
	{
		std::cout << "Your turn!\n What will you do?\n swing: attack with your melee weapon\n";
		std::getline(std::cin, reply); // inventory is unavailable in combat for the time being
		keyword = reply.substr(0, reply.find(" "));

		if (reply == "swing")
		{

			if (floop.size() > 1) // if there's more than one enemy
			{
				while (true) // if the player doesn't pick a legit target loop back
				{
					reply.clear(); // clear the string
					std::cout << "Which target (enter the number)?";
					int i = 1; // pseudo iterator to assign numbers to enemies for easy selection
					int f; // inefficient-laziness iterator
					for (Person *s : floop) // for each bogey
					{
						std::cout << "(" << i << ") " << s->getName() << std::endl; // show off to player and show number tag
						i++; // iterate
					}
					std::cin >> f; // pick a target
					i = 1; // reset
					if (f > floop.size() || f < 1)
					{
						continue;
					}
					else // player picked a valid target
					{
						int damage = Crusader->attack(); // player damage
						std::cout << floop[f]->tankHit(damage) << " damage!\n"; // calculate and apply damage circumstances to the target

						break; // attack is over, break the while(true)
					}
				}
			}
			else // only one bogey
			{
				int damage = Crusader->attack(); // player damage
				Person *s = floop.front(); // pull out the enemy
				s->tankHit(damage); // apply damage
			}

			break; // end turn
		}
		/*else if (reply == "items")
		{
			Crusader->Backpack->contents();
		}*/
		/*else if (keyword == "use")
		{
			break;
		}*/
		/*else if (reply == "kill")
		{
			floop.clear();
		}*/
		else
		{
			std::cout << "Sorry I didn't quite get that.\n";
			reply.clear();
			keyword.clear();
			continue;
		}
	}
}

inline void NPChitter(std::string title) // enemy turn
{
	int bite;
	for (Person *s : floop)
	{
		if (title == s->getName())
		{
			std::cout << title << " attacks!\n";
			bite = s->attack();
			std::cout << Crusader->tankHit(bite) << " damage!\n";

		}
	}
}

void fight() // Come on and slam
{
	// FIGHTING PROTOCOL:
	/*
		All characters are assigned turn order based on 1d6 + Dex Mod
			turns ordered by greatest to least dice value
			ex) one enemy and the player, player gets 6+1dex and enemy gets 5 total, player goes 1st
		On player turn, commands shown as prompt
			swing: attack with physical weapon
			items: shows the usable items in inventory
			use: use item
			cast: show list of available miracles and allow the player to cast one

		Success calculation: Pseudorand (1 - 20 inclusive) + <attacker's dex mod>, must equal or exceed the defender's dex score
		Damage calculation: (<defender armor strength> + <Resilience Mod>) - (<weapon damage> + <Strength mod>)
			Call Person::attack() to calculate damage dealt, return as int
			Call Person::tankHit(int <damage returned from attack>) to deliver the damage and
				calculate resistance

	*/
	std::vector<std::string> turns;
	//std::list<int> rolls;

	// I fully intend on creating a function for turn order and maybe even a whole class for fights
	// however I'm dumbing it all down so I can have a demo build to pass in
	// If you want I can provide a link once I set up a repository on GitHub

	// use vector for turn order (random access allows easy shifting and organizing)
	int i = 0;
	turns.push_back("Crusader");

	// Note to self: Hey moron! push stuff into turns before assuming there's stuff in it!

	std::cout << "You're attacked by ";

	for (Person *s : floop) // roll for each enemy in floop
	{
		std::cout << s->getName() << " ";
		turns.push_back(s->getName());
		// calc turn orders and push the names into 'turns'
		// when someone's turn comes around, pick them up from floop and drop them into 'now'


		// std::cout << description
	}

	std::cout << std::endl;

	bool throwhands = true; // the battle rages on!

	while (throwhands)
	{
		try
		{
			if (i >= turns.size())
			{
				i = 0;
			}
			if (i == 0)
			{
				//std::cout << "Your turn\n";
				PlayerTurn();
			}
			else;
			{
				std::cout << turns[i] << " turn\n";
				NPChitter(turns[i]);
			}
			i++;
			for (int n = 0; n <= floop.size() - 1; n++)
			{
				try
				{

					if (floop[n]->HPnow == 0)
					{
						floop.erase(floop.begin() + (n - 1)); // remove the dead
						turns.erase(turns.begin() + n); // no turns for the fallen
					}
				}
				catch (int goober) // skip over gaps where enemies are already removed
				{
					n++;
					continue;
				}
			}
			if (floop.empty())
			{
				throwhands = false;
			}
		}
		catch (int e) // catch the out-of-bounds exception and reset the iterator
		{
			i = 0;
			//continue;
		} // catch
	} // while 

}

int main()
{
	std::string reply = ""; // declare and initialize the user input string
	std::vector<Room*> KhazadDum; // create a vector for rooms
	Room *Current; // current room

	Crusader->Backpack.Add("flint and steel", true); // add the flint and steel
	// add its places and descriptors
	Crusader->Backpack.ContextAdd(0, "flint and steel", "With your flint and steel you ignite the oil on the wall.\n the flames spread and light the hall, revealing three doors: one to the north, another to the south, and the last in the west.\n");

	//Crusader->Backpack->Add("Holy Bible", true);
	//Crusader->Backpack->Add("Leather Canteen", true);


	// create rooms and push into the vector
	KhazadDum.push_back(new Room("You step down a staircase to the entrance of a great stone room.\n The room is dotted with pillars and you cannot see further than fifty feet into the dark.\n On your left is a small bowl of oil sticking out of the wall. The door slams shut behind you, plunging you into total darkness.\n"));
	KhazadDum.push_back(new Room("Room 1"));
	KhazadDum.push_back(new Room("Fighting Practice\n"));
	KhazadDum.push_back(new Room("Room 3"));
	KhazadDum.push_back(new Room("Room 4"));

	Current = KhazadDum[0]; // we're in this room

	// make connections
							// North, south, west, east
	KhazadDum[0]->Connection(KhazadDum[2], KhazadDum[1], KhazadDum[3], nullptr);
	KhazadDum[1]->Connection(KhazadDum[0], nullptr, nullptr, nullptr);
	KhazadDum[2]->Connection(nullptr, KhazadDum[0], nullptr, nullptr);
	KhazadDum[3]->Connection(KhazadDum[4], nullptr, nullptr, KhazadDum[0]);
	KhazadDum[4]->Connection(nullptr, KhazadDum[3], nullptr, nullptr);

	KhazadDum[0]->mynumber = 0;
	KhazadDum[0]->addUseful("flint and steel");
	KhazadDum[1]->mynumber = 1;
	KhazadDum[2]->mynumber = 2;
	KhazadDum[3]->mynumber = 3;
	KhazadDum[4]->mynumber = 4;

	/*
		map:

			Room 4		Room 2
			Room 3	<- Entrance
						Room 1

			create failsafe for null room
			(going east when there's no room there)
	*/

	KhazadDum[2]->addAxis(0, "practice dummy");
	KhazadDum[4]->addAxis(Baphomet);

	std::cout << "INSTRUCTIONS\n to enter a new room, enter the direction of that room.\n to use an object, type 'use' then the object's name.\n in combat, the commands will be given on screen.\n press any button and hit 'enter' to begin the game!\n";
	std::getline(std::cin, reply);
	reply.clear();

	// set the scene prior to gameplay
	std::cout << "You are a lone swordsman, separated from the other Crusaders\n in a dark Polish forest\n";
	std::cout << "Before you stands a cliff face, carven into a beautifully decorated doorway into a dark cave.\n";
	std::cout << "Rain and night begin to fall, so you enter the cave for shelter.\n";

	while (reply != "quit")
	{
		try // handle unforseen errors
		{
			std::string parsed = " ";
			int parseDelim = 0;
			int length = 0;
			std::string itemName = " ";

			std::cout << Current->getDesc() << std::endl;

			if (Current->AxCount != 0)
			{
				for (int i = 0; i < Current->AxCount; i++)
				{
					floop.push_back(Current->getAxis(i));
				}
			}
			std::getline(std::cin, reply);

			length = reply.length(); // take down the length of the input string
			parseDelim= reply.find(" "); // find a space if possible
			if(parseDelim != std::string::npos) // check for an npos flag and adapt
			{ // there are no spaces in the input
				//std::cout << "\n\n SUCCESSFUL COMMAND PARSE\n\n";
				
				parsed = reply.substr(0, parseDelim); // take a substring of the first word
				itemName = reply.substr(parseDelim+1, length); // take a sub of the remainder


				//std::cout << parsed << std::endl << itemName << "\n\n";
			}
			if (reply == "items")
			{
				Crusader->Holster.contents();
				Crusader->Backpack.contents();
			}
			else if(parsed == "use")
			{
				//std::cout << "\n\n USE IS TRUE, CALLING USEFULEVENT()\n\n";

				if (Current->UsefulEvent(itemName))
				{
					//std::cout << "\n\n USEFULEVENT() IS TRUE CALLING USETHING()\n\n";
					
					Crusader->Backpack.UseThing(itemName, Current->mynumber); // use the item
				}
			}
			else
			{
				if (floop.empty())
				{
					Current = Current->getNext(reply);
				}
				else
				{
					//std::cout << "You've been attacked!\n\n";
					fight();
					//floop.clear();
				}
			}
		}
		catch (int e)
		{

		}
	}

	Crusader->~Player();
	KhazadDum[0]->~Room();
	KhazadDum[1]->~Room();
	KhazadDum[2]->~Room();
	KhazadDum[3]->~Room();
	KhazadDum.clear();
	KhazadDum.~vector();
	Current->~Room();

}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
