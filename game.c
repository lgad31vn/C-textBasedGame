/*
 *Homework#1
 *Nam (Logan) Nguyen
 *CSC322 System Programming
 *First C lab -- program a smal text-based game in C
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Room {
	int roomID;             	//works as room's name
	int roomState;              	//room's state clean - 0, half dirty - 1, dirty - 2
	int creCount ;			//keep track on how many creatures in the room
	struct Room *roomN;     	//neighbor to the North
	struct Room *roomS;		//neighbor to the South
	struct Room *roomE;		//neighbor to the East
	struct Room *roomW; 		//neighbor to the West
	struct Creature *crs[10];	//to keep track on the creatures in the specific room

} Room;

typedef struct Creature{
	int creID; 			//works as creature's name
	int creType;			//creature's type 0-pc, 1-animal, 2-npc
	struct Room *roomLoc;		//room location where creature is in


} Creature;

//global variables
int respectPt = 40; 		//respect level which defines the victory or defeat
Creature *PC; 			//the game is revolving around user
Room* room;
Creature *creatures;


//set up the game
void gameCreation(void){

	int roomAmount;  				//number of room and number of creatures
int state, north, south, east, west; 		//store input values for state of the room and neighbors to N S E W

	printf("How many rooms do you want the game to have? ");  	//prompt the user to input the roomAmout
	scanf("%d", &roomAmount);

	room = (Room*) malloc(roomAmount*sizeof(Room));		//declare room

	printf("Enter 5 integers numbers for each room in order to indicate: State, North, South, East, West\n"); //prompt the user to input the elements to describe the room
	for (int i = 0; i < roomAmount; i++){
		//initialize creCount so it wont get unitialized value by valgrind
		room[i].creCount = 0;

		printf("Room#%d: ",i);
		scanf("%d %d %d %d %d", &state, &north, &south, &east, &west); //input value
		room[i].roomID = i;		//set roomID to i so the roomID would have 0-based index
		room[i].roomState = state;	//roomState


		//check if there are neighbors to the directions N S E W
		if (north == -1){
			room[i].roomN = NULL;			//NULL or no neighbor if input value is -1
		} else {
			room[i].roomN = &room[north];  		//for whatever input value, room[i].roomN points to room[north] -- eg. if north is 2 room[i].roomN would point to room[2]
		}

		//similar to room[i].roomN
		if (south == -1){
			room[i].roomS = NULL;			//NULL or no neighbor if input value is -1
		} else {
			room[i].roomS = &room[south];  		 //room[i].roomS points to room[south]
		}

		if (east == -1){
			room[i].roomE = NULL;			//NULL or no neighbor if input value is -1
		} else {
			room[i].roomE = &room[east];  		//room[i].roomE points to room[east]
		}

		if (west == -1){
			room[i].roomW = NULL;			//NULL or no neighbor if input value is -1
		} else {
			room[i].roomW = &room[west];  		//room[i].roomW points to room[west]
		}
	}

	//CREATURE CREATION

	int creatureAmount;
	int type, roomLocation;

	printf("How many creatures would you like to add to the game? "); //promt the user to input the amount of creatures
	scanf("%d", &creatureAmount);

	creatures = (Creature *) malloc(creatureAmount * sizeof(Creature));	//allocate creatures to the heap

	printf("Enter 2 integer numbers which describe the creatures. (First integer will be either  0-PC, 1-Animal, 2-NPC, meanwhile the second number would be room location where the creature is in)\n");

	//PC = (Creature*) malloc(sizeof(Creature));
	for (int i = 0; i < creatureAmount; i++){
		printf("Cre#%d ", i);
		scanf("%d %d", &type, &roomLocation); 			//2 integers describe type and location
		creatures[i].creID = i;					//assign name for the creatures
		creatures[i].creType = type;				//creTyoe
		creatures[i].roomLoc = &room[roomLocation];		//roomLoc would point to room[roomLocation-index]

		//determine which creature is PC
		if(creatures[i].creType == 0){
			PC = &creatures[i];
			//PC->roomLoc = creatures[i].roomLoc;
		}
	}


	//add creatures to rooms
	for (int i = 0; i < roomAmount; i++){
		for (int j = 0; j < creatureAmount; j++){
			if (creatures[j].roomLoc == &room[i]){
				room[i].crs[room[i].creCount] = &creatures[j];			//creCount works as crs[] size
				room[i].creCount++;						//keep track of creCount to know how many creatures are in the room
			}
		}

	}




}
void look(void);
void clean(Creature *cre);
void dirty(Creature *cre);
void creReaction(Creature *c, int cd);
int nextRoom(Creature *cre, Room* dirRoom);
void cleanDummyCre (Creature* cre, Room *r);
void creAct(Creature *cre, char token[]);
int checkRoomEmpty(Room *room);

//get command from User Input
void userCommand(void){
	char command[15] = "";

	while (strcmp(command, "exit") != 0){

		if (respectPt >= 80) {
			printf("The game ends with the PC in praise. Respect point: %d\n", respectPt);
			break;
		} else if (respectPt <= 0){
			printf("The game ends with the PC in disgrace. Respect point: %d\n", respectPt);
			break;
		} else {

			printf("Enter a command: \n");				//prompt command
			scanf("%s", command);


			if(strcmp(command, "look") == 0){			//look command
				look();
			} else if (strcmp(command, "clean") == 0){		//clean command
				clean(PC);
			} else if (strcmp(command, "dirty") == 0){		//dirty command
				dirty(PC);
			} else if (strcmp(command, "north") == 0){		//move cre to North
				if(!checkRoomEmpty(PC->roomLoc->roomN)){
					printf("Room %d at north is full and get closed! Cannot access! \n", PC->roomLoc->roomN->roomID);
				} else {
					if(nextRoom(PC, PC->roomLoc->roomN)){
						printf("You leave towards north\n");
					} else {
						printf("no room at that direction!\n");
					}
				}
			} else if (strcmp(command, "south") == 0){		//move cre to south
				if(!checkRoomEmpty(PC->roomLoc->roomS)){
					printf("Room %d at south is full and get closed! Cannot access! \n", PC->roomLoc->roomS->roomID);
				} else {
					if(nextRoom(PC, PC->roomLoc->roomS)){
						printf("You leave towards south\n");
					} else {
						printf("no room at that direction!\n");
					}
				}
			} else if (strcmp(command, "east") == 0){		//move cre to east
				if(!checkRoomEmpty(PC->roomLoc->roomE)){
					printf("Room %d at east is full and get closed! Cannot access! \n", PC->roomLoc->roomE->roomID);
				} else {
					if(nextRoom(PC, PC->roomLoc->roomE)){
						printf("You leave towards east\n");
					} else {
						printf("no room at that direction!\n");
					}
				}
			} else if (strcmp(command, "west") == 0){		//move cre to west
				if(!checkRoomEmpty(PC->roomLoc->roomW)){
					printf("Room %d at west is full and get closed! Cannot access! \n", PC->roomLoc->roomW->roomID);
				} else {
					if(nextRoom(PC, PC->roomLoc->roomW)){
						printf("You leave towards west\n");
					} else {
						printf("no room at that direction!\n");
					}
				}
			} else if (strstr(command, ":")) {			//check if there is ":" inside
				char *token = strtok(command, ":");		//first token is actually creID
				int ID = atoi(token);				//atoi function takes integer element
				token = strtok(NULL, ":");			//token is element after :
				for (int i = 0; i < PC->roomLoc->creCount; i++){
					Creature *cre = PC->roomLoc->crs[i];
					if (cre->creID == ID){
						creAct(cre, token);
					}
				}
			} else if (strcmp(command, "help") == 0){	//help command lists out all the commands in the game
				printf("Commands in the game: \n");
				printf("look -- look at the current room which contains PC and print out the room's information\n");
				printf("clean -- clean the current room which change the room's state from dirty to half-dirty, half-dirty  to clean\n");
				printf("dirty -- dirty the current room which change the room's state from clean to half-dirty, half-dirty  to dirty\n");
				printf("north -- move PC to room at north\n");
				printf("south -- move PC to room at south\n");
				printf("east -- move PC to room at east\n");
				printf("west -- move PC to room at west\n");
				printf("creatureID:action -- force NPCs or animals with the same ID to either clean or dirty the room, also move them to the designating directions\n");
				printf("exit -- to quit the game\n");
			} else {
				if(strcmp(command, "exit") == 0){
					continue;
				}
				printf("Invalid input. Type help for more valid commands!\n");
			}
		}
	}
       	printf("Goodbye!\n");

}

void creClean(Creature *cre, Room *r);
void creDirty(Creature *cre, Room *r);

//creature actions
void creAct(Creature *cre, char token[10]){

	if (strcmp(token, "look") == 0){
		look();		// look at the PC's room anyway
	} else if (strcmp(token, "clean") == 0){		//force creatures to clean
		creClean(cre, cre->roomLoc);
	} else if (strcmp(token, "dirty") == 0){		//force creatures to dirty
		creDirty(cre, cre->roomLoc);
	} else if (strcmp(token, "north") == 0){		//force creatures to move to north
		if (nextRoom(cre, cre->roomLoc->roomN)){
			cleanDummyCre(cre, PC->roomLoc);	//if nextRoom returns 1 then gotta clean dummy/null cre.
			printf("You moved %d to room %d at North\n", cre->creID, cre->roomLoc->roomID);
		} else {	//creatures would rebel if it cant move to the desinated directions
			respectPt -=1 ;
			if (cre->creType == 1) {
				printf("%d growls at you. Respect is now %d\n", cre->creID, respectPt);
			} else if (cre->creType == 2) {
				printf("%d grumbles at you. Respect is now %d\n", cre->creID, respectPt);
			}
		}
	} else if (strcmp(token, "south") == 0){
		if (nextRoom(cre, cre->roomLoc->roomS)){
			cleanDummyCre(cre, PC->roomLoc);
			printf("You moved %d to room %d at South\n", cre->creID, cre->roomLoc->roomID);
		} else {
			respectPt -=1 ;
			if (cre->creType == 1) {
				printf("%d growls at you. Respect is now %d\n", cre->creID, respectPt);
			} else if (cre->creType == 2) {
				printf("%d grumbles at you. Respect is now %d\n", cre->creID, respectPt);
			}
		}
	} else if (strcmp(token, "east") == 0){
		if (nextRoom(cre, cre->roomLoc->roomE)){
			cleanDummyCre(cre, PC->roomLoc);
			printf("You moved %d to room %d at East\n", cre->creID, cre->roomLoc->roomID);
		} else {
			respectPt -=1 ;
			if (cre->creType == 1) {
				printf("%d growls at you. Respect is now %d\n", cre->creID, respectPt);
			} else if (cre->creType == 2) {
				printf("%d grumbles at you. Respect is now %d\n", cre->creID, respectPt);
			}
		}
	} else if (strcmp(token, "west") == 0){
		if (nextRoom(cre, cre->roomLoc->roomW)){
			cleanDummyCre(cre, PC->roomLoc);
			printf("You moved %d to room %d at West\n", cre->creID, cre->roomLoc->roomID);
		} else {
			respectPt -=1;
			if (cre->creType == 1) {
				printf("%d growls at you. Respect is now %d\n", cre->creID, respectPt);
			} else if (cre->creType == 2) {
				printf("%d grumbles at you. Respect is now %d\n", cre->creID, respectPt);
			}
		}
	}
}





//get roomStatus
char *getRoomStatus(int roomState) {
	char *roomStatus;					//pointer at roomStatus
	//int PCstate = PCroom->roomState;
	if (roomState == 0){
		roomStatus = "clean";
	} else if (roomState == 1) {
		roomStatus = "half-dirty";
	} else {
		roomStatus =  "dirty";
	}
	return roomStatus;


}
//print neighbors
typedef enum {
	North, South, East, West

} neighbors;

void roomNeighbors(neighbors ne){
	if (ne == North){
		printf("%d to the North, ", PC->roomLoc->roomN->roomID);
	}if (ne == South){
		printf("%d to the South, ", PC->roomLoc->roomS->roomID);
	}if (ne == East){
		printf("%d to the East, ", PC->roomLoc->roomE->roomID);
	}if (ne == West){
		printf("%d to the West, ", PC->roomLoc->roomW->roomID);
	}
}




//commands look
void look (void) {
	//print room's ID and room state
	printf("Room %d, %s, neighbor ",PC->roomLoc->roomID, getRoomStatus(PC->roomLoc->roomState));

	//print neighbors
	if (PC->roomLoc->roomN != NULL){ 					//not null means there is neighbor!
		roomNeighbors(North);
	}
	if (PC->roomLoc->roomS != NULL) {
		roomNeighbors(South);
	}

	if (PC->roomLoc->roomE != NULL) {
		roomNeighbors(East);
	}
	if (PC->roomLoc->roomW != NULL) {
		roomNeighbors(West);
	}

	//print creatures in the room
	printf(" contains: \n");

	for (int i = 0; i < PC->roomLoc->creCount; i++){	//loop through creature array

		Creature *cre = PC->roomLoc->crs[i];
		if (cre->creType == 0) {
			printf("PC %d\n", PC->creID);
		}else if (cre->creType == 1){
			printf("Animal %d\n", cre->creID);
		} else if (cre->creType == 2){
			printf("NPC %d\n", cre->creID);
		}
	}

	//little more informations
	printf("Room's creature amount: %d\n", PC->roomLoc->creCount);
	printf("PC's respect point: %d\n", respectPt);


}


//clean command
void clean(Creature *cre) {
 	int state = cre->roomLoc->roomState;
	if(state == 0) {
		printf("'Clean' command does not affect the room because it is already clean\n");		//if room is already clean, 'clean' would not affect
	} else/* if (state == 1 || state == 2)*/{
		state -=1;										//state would be reduced by 1 after an action
		cre->roomLoc->roomState = state;								//re-determine cre's roomState
		creReaction(cre, 0);

	}
	cleanDummyCre(cre, cre->roomLoc);								//clean off dummyCre after the creatures left

}

//dirty command
void dirty(Creature *cre){
	int state = cre->roomLoc->roomState;
	if(state == 2) {
		printf("'Dirty' command does not affect the room because it is already dirty\n");   	//if room is already dirty, 'dlean' would not affect
	} else /*if (state == 1 || state == 0)*/{
		state +=1;										//state would be increased by 1 after an action
		cre->roomLoc->roomState = state;								//re-determine PC's roomState
		creReaction(cre, 1);

	}
	cleanDummyCre(cre, cre->roomLoc);							//clean off dummyCre after the creatures left


}

void creClean(Creature *cre, Room *r){
	if(r->roomState == 0){
		printf("'Clean' command does not affect the room because it is already clean\n");
	} else {
		if(cre->creType == 1){
			respectPt += 3;
			printf("%d licks a lot. Respect is now %d\n", cre->creID, respectPt);
		} else if(cre->creType == 2){
			respectPt -= 3;
			printf("%d grumbles a lot. Respect is now %d\n", cre->creID, respectPt);
		}
		r->roomState -= 1;
		creReaction(cre, 0);
		cleanDummyCre(cre, PC->roomLoc);							//clean off dummyCre after the creatures left
	}
}
void creDirty(Creature *cre, Room *r){
	if(r->roomState == 2){
		printf("'Dirty' command does not affect the room because it is already dirty\n");
	} else {
		if(cre->creType == 1){
			respectPt -= 3;
			printf("%d growls a lot. Respect is now %d\n", cre->creID, respectPt);
		} else if(cre->creType == 2){
			respectPt += 3;
			printf("%d smiles a lot. Respect is now %d\n", cre->creID, respectPt);
		}
		r->roomState += 1;
		creReaction(cre, 1);
		cleanDummyCre(cre, PC->roomLoc);							//clean off dummyCre after the creatures left
	}
}


//clean off dummy creatures who sit on leaving creatures
void cleanDummyCre(Creature *cre, Room *r){
	int j = 0;
	for (int i = 0; i < r->creCount; i++){
		if(r->crs[i] != NULL){
			r->crs[j] = r->crs[i];
			j++;
		}

	}
	r->creCount = j;
}


void creMove(Creature *cre);

//creatuer reation
void creReaction(Creature *cre, int cd){				//cd shorts for clean-0, dirty-1
	if (cre->creType == 1){
		if(cre->roomLoc->roomState == 2){
			creMove(cre);
			cleanDummyCre(cre, PC->roomLoc);
		}

	} else if (cre->creType == 2){
		if (cre->roomLoc->roomState == 0){
			creMove(cre);
			cleanDummyCre(cre, PC->roomLoc);
		}
	}

	for (int i = 0; i < PC->roomLoc->creCount; i++){
		Creature *c = PC->roomLoc->crs[i];

		if (cd == 0 && c->creType == 1) {
			if(c->creID == cre->creID){
				continue;
			} else {
				respectPt ++;									//keep updating respectPt
				printf("Animal %d licks. Respect is now %d\n", c->creID, respectPt);		//Animal & NPC would react differently based on the actions
			}
		} else if (cd == 0 && c->creType == 2) {
			if(c->creID == cre->creID){
				continue;
			} else {
				respectPt --;
				printf("NPC %d grumbles. Respect is now %d\n", c->creID, respectPt);
				if (c->roomLoc->roomState == 0) {
					creMove(c);
				}
			}
		}else if (cd == 1 && c->creType == 2) {
			if(c->creID == cre->creID){
				continue;
			} else {
				respectPt ++;
				printf("NPC %d smiles. Respect is now %d\n", c->creID, respectPt);
			}

		} else if (cd == 1 && c->creType == 1) {
			if(c->creID == cre->creID){
				continue;
			} else {
				respectPt --;
				printf("Animal %d growls. RespectPt is now %d\n", c->creID, respectPt);
				if (c->roomLoc->roomState == 2) {
					creMove(c);
				}
			}
		}
	}


}
void removeFromOldRoom(Creature *cre, Room* r);
void leftThruCeiling(Creature *cre);

//function move creature
void creMove(Creature *cre){
	int randDir = rand()%4;
	switch (randDir){
		case 0:
			if (nextRoom(cre, cre->roomLoc->roomN)){
				printf("%d left toward north\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomS)){
				printf("%d left toward south\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomE)){
				printf("%d left toward east\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomW)){
				printf("%d left toward west\n", cre->creID);
			} else {
				leftThruCeiling(cre); 					//creature disappear from the simulation
			}
			break;
		case 1:
			if (nextRoom(cre, cre->roomLoc->roomS)){
				printf("%d left toward south\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomE)){
				printf("%d left toward east\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomW)){
				printf("%d left toward west\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomN)){
				printf("%d left toward north\n", cre->creID);
			} else {
				leftThruCeiling(cre); 					//creature disappear from the simulation
			}
			break;

		case 2:
			if (nextRoom(cre, cre->roomLoc->roomE)){
				printf("%d left toward east\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomW)){
				printf("%d left toward west\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomN)){
				printf("%d left toward north\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomS)){
				printf("%d left toward south\n", cre->creID);
			} else {
				leftThruCeiling(cre); 					//creature disappear from the simulation
			}
			break;
		case 3:
			if (nextRoom(cre, cre->roomLoc->roomW)){
				printf("%d left toward west\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomN)){
				printf("%d left toward north\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomS)){
				printf("%d left toward south\n", cre->creID);
			} else if (nextRoom(cre, cre->roomLoc->roomE)){
				printf("%d left toward east \n", cre->creID);
			} else {
				leftThruCeiling(cre); 					//creature disappear from the simulation
			}
			break;


	}

}
void creSelfCD(Creature *cre);
void addCreToNewRoom(Creature *cre, Room *r);
//int checkRoomEmpty(Room *room);

//check if cre can move to the next room or not
int nextRoom(Creature *cre, Room *dirRoom){
	if (dirRoom == NULL  || !checkRoomEmpty(dirRoom)){
		return 0;
	}

	removeFromOldRoom(cre, cre->roomLoc);
	addCreToNewRoom(cre, dirRoom);					//add to new room
	creSelfCD(cre);							//make the room's state change to satisfy creature
	cleanDummyCre(cre, cre->roomLoc);				//clean off dummyCre after the creatures left

	return 1;

}

//remove cre from its room
void removeFromOldRoom(Creature *cre, Room* r){
	for (int i = 0; i < r->creCount; i++){
		if(r->crs[i] != NULL){
			if (cre->creID == r->crs[i]->creID){
				r->crs[i] = NULL;
				cre->roomLoc = NULL;
				break;

			}
		}
	}

}


//add creatures to the new rooms
void addCreToNewRoom(Creature *cre, Room *r){
	cre->roomLoc = r;				//assign roomLoc of the creature to a new room
	r->crs[r->creCount] = cre;			//add creature cre to the last element of the crs array
	r->creCount = r->creCount + 1;				//keep crecount of the room updated

}

//make creature clean or dirty the room itself so that it can stay
void creSelfCD(Creature *cre){
	if (cre->creType == 1 && cre->roomLoc->roomState == 2){
		cre->roomLoc->roomState -= 1;
	} else if (cre->creType == 2 && cre->roomLoc->roomState == 0){
		cre->roomLoc->roomState += 1;
	}
}


//function leftThruCeiling
void leftThruCeiling(Creature *cre){
	printf("%d left through the ceiling! \n", cre->creID);
	printf("Because you let %d leave through the ceiling, \n", cre->creID);

	removeFromOldRoom(cre, PC->roomLoc);
	for (int i = 0; i < PC->roomLoc->creCount; i++){
		if(PC->roomLoc->crs[i] == NULL){
			continue;
		} else {
			if (PC->roomLoc->crs[i]->creType == 1){
				respectPt -=1;
				printf("	Animal %d growls. Respect is now %d\n", PC->roomLoc->crs[i]->creID, respectPt);
			} else if (PC->roomLoc->crs[i]->creType == 2){
				respectPt -=1;
				printf("	NPC %d grumbles. Respect is now %d\n", PC->roomLoc->crs[i]->creID, respectPt);
			}
		}
	}
}


//check if a Room is full or not
int checkRoomEmpty(Room *room){
	if (room->creCount == 10) {
		return 0;				//return true if creCount == 10. Room is full
	}
		return 1;				//return false if creCount < 10. Room is not full
}

int main(){

	gameCreation();					//create the game
	userCommand();


	//free elements which have been allocated on the heap
	free(room);
	free(creatures);

	return 0;
}










