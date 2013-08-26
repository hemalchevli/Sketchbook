/* 
FourBitMaze - generates and allows play with Oskar van Deventer's 4-bit mazes
Author: Todd W. Neller
See: http://cs.gettysburg.edu/~tneller/mazes/oskar4bit/arduino.html

In this variation of the four-bit maze, one is presented with four buttons and four lit LEDs.  The goal is
to turn off all LEDs. Pressing a button by an unlit LED has no effect.  Pressing a button by a lit LED has
the effect of turning the LED off and changing the state of other LEDs.  (The change is always the same
for a given LED pattern and button press.)  Two additional buttons allow one to reset the current maze to
the initial state, or generate an entirely new maze. 

In the four bit maze, there are four types of constraints:
1. Selecting an "off" bit causes no state change.
2. Selecting an "on" bit turns that bit off.
3. No two transitions out of a state lead to the same state.
4. (a) All states are reachable from the initial state, and (b) the goal state is reachable from all states.) 
Let us call these constraints of (4) the "reachability" constraints.

This is intended to be a simple, fun, introductory Arduino project.  There is also potential for 
interesting research on maze generation or the development of measures of maze quality.  Enjoy!

For more about Oskars 4-bit mazes, see:
http://cs.gettysburg.edu/~tneller/mazes/oskar4bit/index.html
http://www.clickmazes.com/fourbit/ix4bit.htm

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a copy of the GNU General Public License see <http://www.gnu.org/licenses/>.
*/

// hardware-related definitions
#define BUTTON_0 6 // pin for button 0
#define BUTTON_1 5 // pin for button 1
#define BUTTON_2 4 // pin for button 2
#define BUTTON_3 3 // pin for button 3
#define RESET 8 // pin for reset button 
#define NEW_MAZE 2 // pin for new maze button 
#define LED_0 9 // pin for LED 0
#define LED_1 10 // pin for LED 1
#define LED_2 11 // pin for LED 2
#define LED_3 12 // pin for LED 3
#define DEBOUNCE_INTERVAL 100 // minimum interval between button reads
#define VERBOSE true // display status and maze information 
int inputPins[] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, RESET, NEW_MAZE}; // input pins
int outputPins[] = {LED_0, LED_1, LED_2, LED_3}; // output pins

// software-related definitions
#define NULL -1 // denoted unassigned value
#define bits 4 // number of bits
#define states 16 // number of states represented by the bits (2^bits)
boolean state1Bits[bits]; // temporary storage for bits of integers
boolean state2Bits[bits]; // temporary storage for bits of integers
int nextState[states * bits]; // a row-major 1D ordering of a 2D array mapping states (rows) and bits (columns) to next states (or NULL if no transition)
int goalDistance[states]; // minimum number of transitions from each state to the goal; NULL means goal is unreachable from that state
boolean isReachable[states]; // whether or not a given state can be reached from the initial state
boolean isReaching[states]; // whether or not the goal can be reached from a given state
int stateQueue[states]; // temporary storage for breadth-first maze traversal
int score = 0; // a measure of "badness" of the current maze in generation
int maxAllowableScore = 8; // the highest "badness" we accept to terminate maze generation
int prevState, prevBit, prevNextState; // information to restore a changed transition in maze generation
int state; // current state

/********************************************************************************************************/
void setup() { // initialize communications, variables, and pins; generate maze
  if (VERBOSE) {
    Serial.begin(9600);
    Serial.println("Initializing...");
  }

  // initialize variables 
  for (int i = 0; i < states; i++) {
    for (int j = 0; j < bits; j++) { // no transitions
      setNextState(i, j, NULL);
    }
    isReachable[i] = false; // Nothing initially is reachable or reaches the goal...
    isReaching[i] = false;
    goalDistance[i] = NULL;
  }
  isReachable[states - 1] = true; // Except the initial state is reachable, ...
  isReaching[0] = true; // and the goal state reaches itself with distance 0. 
  goalDistance[0] = 0;

  // generate maze
  randomSeed(42); // Don't panic.
  createRandomMaze(); 

  // setup pins
  for (int i = 0; i < 6; i++)
    pinMode(inputPins[i], INPUT);    
  for (int i = 0; i < 4; i++)
    pinMode(outputPins[i], OUTPUT);

  // initialize maze state
  state = states - 1;
  displayState();
}

/********************************************************************************************************/
void loop() { // read and process button presses
  int button = NULL;

  // wait until no button is being pressed
  if (VERBOSE) Serial.println("Waiting for no button");
  boolean done = false;
  while (!done) {
    done = true;
    for (int i = 0; done && i < 6; i++)
      done = digitalRead(inputPins[i]) == HIGH;
  }

  // wait until button is pressed
  if (VERBOSE) Serial.println("Waiting for button");
  while (button == NULL) {
    for (int i = 0; button == NULL && i < 6; i++)
      if (digitalRead(inputPins[i]) == LOW)
        button = i;
  }

  // process button press
  if (VERBOSE) { 
    Serial.print("Button: "); 
    Serial.println(button); 
  }
  if (button >= 0 && button <= 3) { // attempted state transition
    // attempt state change
    int next = getNextState(state, button);
    if (next != NULL) { // successful transition
      if (VERBOSE) {
        Serial.print("Transition: "); 
        Serial.print(state); 
        Serial.print("("); 
        Serial.print(button); 
        Serial.print(")"); 
        Serial.println(next);
      }
      state = next;
    } 
    else if (VERBOSE) { // unsuccessful transition
      Serial.print("No transition: "); 
      Serial.print(state); 
      Serial.print("("); 
      Serial.print(button); 
      Serial.print(")"); 
      Serial.println(state);
    }
  } 
  else if (button == 4) { // reset to initial state of maze
    // reset state
    if (VERBOSE) Serial.println("Reset");
    strobeOutput();
    state = states - 1;
  } 
  else if (button == 5) { // generate a new maze and reset
    // new maze
    strobeOutput();
    int seed = millis();
    if (VERBOSE) {
      Serial.print("New Maze ");
      Serial.println(seed);
    }
    randomSeed(seed); // After first maze, mazes are seeded by time.
    createRandomMaze();
    state = states - 1;
  } 
  else 
    // should not be here - signal error
    if (VERBOSE) Serial.println("Error - illegal button press");
 

  displayState(); // update state LED display

  delay(DEBOUNCE_INTERVAL); // delay for button debounce
}

/********************************************************************************************************/
void strobeOutput() { // strobe LEDs to confirm reset/new maze button press; set state to 0
  for (int i = 0; i < 3; i++) {
    state = states - 1;
    displayState();
    delay(50);
    state = 0;
    displayState();
    delay(50);
  }
}

/********************************************************************************************************/
void displayState() { // update the LED output to reflect the current maze state
  convertIntToBits(state, state1Bits);
  for (int i = 0; i < bits; i++) // set LED output according to current state bits
    if (state1Bits[i])
      digitalWrite(outputPins[i], HIGH);
    else
      digitalWrite(outputPins[i], LOW);
}

/********************************************************************************************************/
void createRandomMaze() { // create random four-bit maze
/* 
In the four bit maze, there are four types of constraints:
1. Selecting an "off" bit causes no state change.
2. Selecting an "on" bit turns that bit off.
3. No two transitions out of a state lead to the same state.
4. (a) All states are reachable from the initial state, and (b) the goal state is reachable from all states.) 
Let us call these constraints of (4) the "reachability" constraints.

Algorithm:
- Generate a random four-bit maze without the reachability constraints. 
- While these reachability constraints are not met, or the minimum length path from the initial state to
  the goal state is less than 8: 
  -- Randomly change a transition out of a state, respecting all transition constraints except 
     reachability constraints. 
  -- If this change results in an increase in the number of unreachable states plus the number of states
     from which the goal is unreachable, or if this number is unchanged and the minimum path length from
     the initial state to the goal state decreases, undo this transition change. 

No doubt, interested readers could devise a better maze generation algorithm, or at least a better
termination condition.  For example, one might make use of the average goal distance, or create a model
of maze traversal and optimize different properties.  Puzzling over puzzle generation is itself fun!
*/
  
  randomizeTransitions();  // randomize maze without enforcing reachability constraints
  computeScore();
  while (score > maxAllowableScore) { // while reachability constraints aren't met and
                                      // the minimum solution path is too short
    changeRandomTransition(); // Change a transition.
    int oldScore = score;
    computeScore();
    if (score > oldScore) { // If the change was for the worse, ...
      setNextState(prevState, prevBit, prevNextState); // ... restore the previous transition (and score).
      score = oldScore;
    }
  }
  if (VERBOSE) {
    Serial.println("Maze generated.");
    printMaze();
  }
}

/********************************************************************************************************/
boolean canTransition(int bit, int state1, int state2) { // compute whether selecting a given bit in
                                                         // state1 could lead to state2 
  convertIntToBits(state1, state1Bits);
  convertIntToBits(state2, state2Bits);
  return state1Bits[bit] && !state2Bits[bit]; // constraints 1 and 2 (above) are met
}

/********************************************************************************************************/
void randomizeTransitions() { // Randomize maze transitions according to constraints 1 - 3 (above).
  for (int s = 1; s < states; s++) { // For each state ...
    convertIntToBits(s, state1Bits);
    for (int b = 0; b < bits; b++) // ... and for each "on" bit of that state (constraint 1), ...
      if (state1Bits[b]) {
        setNextState(s, b, NULL);
        while (getNextState(s, b) == NULL) {
          int s2 = (int) random(states); // ... generate a random successor state ...
          convertIntToBits(s2, state2Bits);
          boolean isCandidate = !state2Bits[b]; // ... that satisfies turns "off" bit (constraint 2) ... 
          for (int b2 = 0; isCandidate && b2 < b; b2++) // ... and doesn't duplicate another transition
                                                        // from that state (constraint 3).
            if (getNextState(s, b2) == s2)
              isCandidate = false;
          if (isCandidate)
            setNextState(s, b, s2);
        }
      }
  }
}

/********************************************************************************************************/
void changeRandomTransition() { // Pick a random transition and change it according to constraints 1-3 (above).
  // Together, prevState, prevBit, and prevNextState will contain all necessary info to undo the change.
  prevState = (int) random(1, states); // Pick a random non-goal state. (The goal has no transitions.)
  convertIntToBits(prevState, state1Bits);
  prevBit = (int) random(bits); // Select a random transitioning ("on") bit of the state.
  while (getNextState(prevState, prevBit) == NULL)
    prevBit = (int) random(bits);
  prevNextState = getNextState(prevState, prevBit); 
  boolean done = false;
  while (!done) {
    int s2 = (int) random(states); // Select a random next state ...
    done = true;
    if (s2 == prevNextState) // ... that is a change from the previous transition, ...
      done = false;
    else if (!canTransition(prevBit, prevState, s2)) // ... respects constraints 1 and 2 (legal transition), and ...
      done = false;
    else { // ... and respects constraint 3 (non-duplicated transition).
      for (int b2 = 0; done && b2 < bits; b2++)
        if (getNextState(prevState, b2) == s2)
          done = false;
    }
    if (done)
      setNextState(prevState, prevBit, s2);
  }
}

/********************************************************************************************************/
void computeScore() { // compute a "badness" score for the current maze
/* 
Since the maximum possible minimum distance from the initial state to the goal state is (states - 1),
multiplying other factors by (states) gives them a higher priority in the scoring.
We
- begin with the number of states not reachable from the initial state,
- add the number of states that cannot reach the goal state,
- multiply this sum by (states) to give them top priority, and
- add (states) minus the minimum distance from the initial state to the goal state.
Thus minimizing this score (1) satisfies reachability constraints (constraint 4), and 
(2) increases the minimum distance to the goal state.
*/
  score = states - computeReachable();
  score += states - computeReaching();
  score *= states;
  score += states - goalDistance[states - 1];
}

/********************************************************************************************************/
int computeReachable() { // perform a breadth first search to mark states reachable from the initial state
                         // and return the number of reachable states
  for (int i = 0; i < states - 1; i++) // mark all but the initial state unreachable 
    isReachable[i] = false;
  int head = 0;
  int length = 1;
  stateQueue[0] = states - 1;
  while (head < length) { // While there are still states to search ...
    int s = stateQueue[head]; // ... select the first unsearched, ...
    for (int b = 0; b < bits; b++) { // ... follow all transitions, and ... 
      int s2 = getNextState(s, b);
      if (s2 != NULL && !isReachable[s2]) { // ... add states that haven't been reached yet to the queue.
        isReachable[s2] = true;
        stateQueue[length++] = s2;
      }
    }
    head++;
  }
  return length;
}

/********************************************************************************************************/
int computeReaching() { // iteratively compute and return the number of all states
  int reachCount = 1;
  for (int i = 1; i < states; i++) { // mark all states except the goal state as not reaching the goal
    isReaching[i] = false;
    goalDistance[i] = NULL;
  }
  int distance = 0; // Begin with the goal state (distance 0)
  boolean newReaching = true;
  while (newReaching) { // While new reaching states are still being found, ...
    newReaching = false;
    for (int s = 1; s < states; s++) {
      for (int b = 0; !isReaching[s] && b < bits; b++) {
        int s2 = getNextState(s, b);
        // ... mark all unreaching states that reach states of the previous iteration (distance)
        // as reaching with a distance one greater.  Note that a new reaching state is found.
        if (s2 != NULL && isReaching[s2] && goalDistance[s2] == distance) {
          newReaching = true;
          isReaching[s] = true;
          goalDistance[s] = goalDistance[s2] + 1;
          reachCount++;
        }
      }
    }
    distance++; // increase distance to the distance of the newly found states
  }
  return reachCount;
}

/********************************************************************************************************/
void convertIntToBits(int s, boolean stateBits[]) { // write the bits of state s to a boolean array
  int tmp = s;
  for (int b = 0; b < bits; b++) {
    stateBits[b] = (tmp % 2) == 1;
    tmp /= 2;
  }	
}

/********************************************************************************************************/
int getNextState(int state, int bit) { // return the next state transitioned to by the given bit in the 
                                       // given state or NULL if there is no transition.
  return nextState[state*bits + bit];
}

/********************************************************************************************************/
void setNextState(int state, int bit, int value) { // set the next state transitioned to by the given
                                                   // bit in the given state or NULL if no transition.
  nextState[state*bits + bit] = value;
}

/********************************************************************************************************/
void printMaze() { // serially transmit a text representation of the maze
/* 
On each line is
- a maze state (integer), 
- the parenthesized minimum distance to goal or NULL,
- a colon, and
- the next states (or NULL) associated with each bit of that state.
*/
  Serial.println();
  for (int s = 0; s < states; s++) {
    Serial.print(s); 
    Serial.print("("); 
    Serial.print(goalDistance[s]); 
    Serial.print("): ");
    for (int b = 0; b < bits; b++) {
      Serial.print(getNextState(s, b)); 
      Serial.print(" ");
    }
    Serial.println();
  }
}

