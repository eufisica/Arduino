#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Servo myServo;

const int btn1 = 10, btn2 = 9, btn3 = 8;
const int servoPin = 11, trigPin = 12, echoPin = 13;

const unsigned long STUDY_DURATION_MS = 25ULL * 60ULL * 1000ULL;
const unsigned long OVERRIDE_HOLD_TIME = 3000;
const unsigned long SKIP_HOLD_TIME = 4000;
const unsigned long DEBOUNCE_TIME = 50;
const unsigned long ANIMATION_FRAME_TIME = 400;
const unsigned long REMOVAL_BUFFER_TIME = 2000;
const unsigned long UNLOCK_COUNTDOWN = 5;
const unsigned long ANGRY_DISPLAY_TIME = 3000;
const unsigned long LOCKING_COUNTDOWN = 5;
const unsigned long DISTANCE_CHECK_INTERVAL = 100;
const unsigned long BLINK_INTERVAL = 3000;

const int DIST_PHONE_REMOVED = 18;
const int DIST_PHONE_DETECTED = 5;
const int DIST_IDLE_THRESHOLD = 8;

// State variables
unsigned long lockStartTime = 0;
bool isLocked = false;
bool phoneRemoved = true; 
unsigned long lastDistanceCheck = 0;
long lastKnownDistance = 999;

int password[3], inputSeq[3], inputIdx = 0;
bool challengeActive = false;

// Button states
unsigned long btn1LastPress = 0, btn2LastPress = 0, btn3LastPress = 0;
bool btn1Pressed = false, btn2Pressed = false, btn3Pressed = false;
bool btn1JustPressed = false, btn2JustPressed = false, btn3JustPressed = false;

unsigned long overrideStartTime = 0;
bool overrideActive = false;
unsigned long skipStartTime = 0;
bool skipActive = false;

// Animation
unsigned long lastAnimationUpdate = 0;
int animationFrame = 0;
unsigned long lastBlink = 0;

unsigned long removalCheckStart = 0;
bool checkingRemoval = false;
unsigned long lockingSequenceStart = 0;
bool lockingInProgress = false;
unsigned long angryFaceStart = 0;
bool showingAngry = false;

unsigned long unlockSequenceStart = 0;
bool unlockSequenceActive = false;
bool waitingForSleepAfterTimeUp = false;
long lastDistanceDuringChallenge = 999;
long initialDistanceAtChallengeStart = 999;
long minDistanceDuringChallenge = 999;
bool phoneWasTakenAfterChallenge = false;
bool showingAngryInChallenge = false;
unsigned long angryFaceStartInChallenge = 0;

// --- PIXEL ART ---
// Eyes (two-row style for blinking)
byte eyeTop[8] = {0,14,31,31,31,31,31,31};      // Top half of eye
byte eyeBottom[8] = {31,31,31,31,31,31,14,0};   // Bottom half of eye
byte eyeClosed[8] = {0,0,0,0,31,31,0,0};       // Closed eye

// Sleeping
byte sleepEye1[8] = {0,0,0,14,14,0,0,0};
byte sleepEye2[8] = {0,0,0,0,31,31,0,0};
byte sleepEye3[8] = {0,0,0,7,7,0,0,0};
byte tinyMouth[8] = {0,0,0,0,0,0,0,4};         // Tiny dot mouth (one pixel)

// Happy
byte happyMouth[8] = {0,0,0,17,17,17,10,4};
byte cuteMouth[8] = {0,0,0,0,0,0,0,4}; // Tiny cute mouth (smaller than happy)
byte smallEye[8] = {0,0,0,14,31,31,14,0}; // Smaller round eye

// Angry
byte angryEyeTop[8] = {0,0,3,7,15,31,31,31};
byte angryEyeBottom[8] = {31,31,31,15,7,3,0,0};
byte angryMouth[8] = {0,0,0,0,0,4,10,17};

// Suspicious
byte suspiciousEyeTop[8] = {0,0,0,7,14,7,0,0};
byte suspiciousEyeBottom[8] = {0,0,0,7,14,7,0,0};

// Surprised
byte surpriseEyeTop[8] = {0,0,14,31,31,31,31,31};
byte surpriseEyeBottom[8] = {31,31,31,31,31,14,0,0};
byte surpriseMouth[8] = {0,0,0,14,10,14,0,0};

// Focus
byte focusEyeTop[8] = {0,14,31,31,31,31,31,31};
byte focusEyeBottom[8] = {31,31,31,31,31,31,14,0};
byte neutralMouth[8] = {0,0,0,0,0,0,0,0};

// Reward (heart eyes)
byte heartEyeTop[8] = {0,10,31,31,31,31,14,0};
byte heartEyeBottom[8] = {0,0,14,31,31,31,10,0};
byte rewardMouth[8] = {0,0,0,17,17,17,10,4};

// Shy
byte shyEyeTop[8] = {0,0,0,7,14,7,0,0};
byte shyEyeBottom[8] = {0,0,0,7,14,7,0,0};

// Dizzy (X eyes)
byte xEyeTop[8] = {27,14,4,10,17,10,4,14};
byte xEyeBottom[8] = {27,14,4,10,17,10,4,14};

byte progressBar[8] = {31,31,31,31,31,31,31,31};

void setup() {
  lcd.begin(16, 2);
  randomSeed(analogRead(A1));
  myServo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  unlockBox(); 
  lastAnimationUpdate = millis();
  lastBlink = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  updateButtonStates();
  
  if (currentTime - lastDistanceCheck >= DISTANCE_CHECK_INTERVAL) {
    long dist = getDistance();
    if (dist < 200) lastKnownDistance = dist;
    lastDistanceCheck = currentTime;
  }
  long dist = lastKnownDistance;

  handleMasterOverride();
  if (isLocked) handleSkipTimer();

  // Only show "take phone" if phone is actually detected in box after unlock
  if (!phoneRemoved && dist <= DIST_PHONE_REMOVED) {
    showRemovalPrompt(dist, currentTime);
    return;
  }

  // If phone was supposed to be removed but it's not detected, go to sleeping
  if (!phoneRemoved && dist > DIST_PHONE_REMOVED) {
    phoneRemoved = true;
    checkingRemoval = false;
    unlockSequenceActive = false;
  }

  if (!isLocked && dist > DIST_IDLE_THRESHOLD) {
    showSleepingAnimation(currentTime);
  } 
  // FIX: Only start locking sequence if phone is close AND we're NOT in the "just unlocked" state
  else if (!isLocked && dist <= DIST_PHONE_DETECTED && phoneRemoved && !waitingForSleepAfterTimeUp) {
    // Check if we just unlocked (phone was recently taken out)
    // We know the phone was taken out if distance is now short (phone is back) 
    // AND phoneRemoved was true (meaning phone was out)
    bool canLock = false;
    
    if (!phoneWasTakenAfterChallenge) {
      // Normal case - phone never taken during challenge, can lock immediately
      canLock = true;
    } else {
      // After challenge - phone was taken during challenge
      // Only lock if phone has been back for a while (10 seconds)
      static unsigned long phoneBackTime = 0;
      if (dist <= DIST_PHONE_DETECTED) {
        if (phoneBackTime == 0) {
          phoneBackTime = currentTime; // Start timer when phone is put back
        }
        if (currentTime - phoneBackTime >= 10000) {
          canLock = true;
        }
      } else {
        // Phone taken out again, reset timer
        phoneBackTime = 0;
      }
    }
    
    if (canLock) {
      if (!lockingInProgress) {
        lockingSequenceStart = currentTime;
        lockingInProgress = true;
        lcd.clear();
      }
      runLockingSequence(currentTime);
    }
  } else if (isLocked) {
    unsigned long elapsed = currentTime - lockStartTime;
    if (elapsed < STUDY_DURATION_MS) {
      // FIXED: Check for cheating and show angry expression properly
      if ((btn1JustPressed || btn3JustPressed) && !skipActive && !challengeActive) {
        // Only show angry if not already showing
        if (!showingAngry) {
          showRandomCheatingExpression();
          // Don't show studying face when angry is shown
          return; // ADDED: Return to prevent overwriting angry face
        }
      }
      
      // Only show studying face if not showing angry expression
      if (!showingAngry) {
        showStudyingFace(elapsed, currentTime);
      }
      
      // Clear angry face after timeout
      if (showingAngry && (currentTime - angryFaceStart >= ANGRY_DISPLAY_TIME)) {
        showingAngry = false;
        lcd.clear();
      }
    } else {
      handlePasswordChallenge(currentTime, dist);
    }
  }
}

void updateButtonStates() {
  unsigned long currentTime = millis();
  
  // Button 1
  bool btn1State = (digitalRead(btn1) == LOW);
  if (btn1State && !btn1Pressed && (currentTime - btn1LastPress > DEBOUNCE_TIME)) {
    btn1Pressed = true;
    btn1JustPressed = true;
    btn1LastPress = currentTime;
  } else {
    btn1JustPressed = false;
    if (!btn1State) btn1Pressed = false;
  }
  
  // Button 2
  bool btn2State = (digitalRead(btn2) == LOW);
  if (btn2State && !btn2Pressed && (currentTime - btn2LastPress > DEBOUNCE_TIME)) {
    btn2Pressed = true;
    btn2JustPressed = true;
    btn2LastPress = currentTime;
  } else {
    btn2JustPressed = false;
    if (!btn2State) btn2Pressed = false;
  }
  
  // Button 3
  bool btn3State = (digitalRead(btn3) == LOW);
  if (btn3State && !btn3Pressed && (currentTime - btn3LastPress > DEBOUNCE_TIME)) {
    btn3Pressed = true;
    btn3JustPressed = true;
    btn3LastPress = currentTime;
  } else {
    btn3JustPressed = false;
    if (!btn3State) btn3Pressed = false;
  }
}

void handleMasterOverride() {
  unsigned long currentTime = millis();
  if (btn1Pressed && btn3Pressed) {
    if (!overrideActive) {
      overrideStartTime = currentTime;
      overrideActive = true;
    } else if (currentTime - overrideStartTime >= OVERRIDE_HOLD_TIME) {
      unlockBox();
      overrideActive = false;
    }
  } else {
    overrideActive = false;
  }
}

void handleSkipTimer() {
  unsigned long currentTime = millis();
  if (btn2Pressed) {
    if (!skipActive) {
      skipStartTime = currentTime;
      skipActive = true;
    } else if (currentTime - skipStartTime >= SKIP_HOLD_TIME) {
      completeStudySession();
      skipActive = false;
    }
  } else {
    skipActive = false;
  }
}

void completeStudySession() {
  lockStartTime = millis() - STUDY_DURATION_MS;
  challengeActive = false;
  lcd.clear();
}

void showRemovalPrompt(long dist, unsigned long currentTime) {
  // Only show if phone is actually detected (close distance)
  if (dist > DIST_PHONE_REMOVED) {
    // Phone already removed, go to sleeping
    phoneRemoved = true;
    checkingRemoval = false;
    unlockSequenceActive = false;
    return;
  }
  
  // Phone is in box - show cute "take phone" face
  loadCuteFace();
  drawFace((currentTime / 300) % 2, "TAKE PHONE!");
  
  // Check if phone is being removed
  if (dist > DIST_PHONE_REMOVED) {
    if (!checkingRemoval) {
      removalCheckStart = currentTime;
      checkingRemoval = true;
    } else if (currentTime - removalCheckStart >= REMOVAL_BUFFER_TIME) {
      // Phone confirmed removed - start unlock countdown
      if (!unlockSequenceActive) {
        unlockSequenceStart = currentTime;
        unlockSequenceActive = true;
        lcd.clear();
      }
      
      unsigned long elapsed = currentTime - unlockSequenceStart;
      int remaining = UNLOCK_COUNTDOWN - (elapsed / 1000);
      
      if (remaining > 0) {
        loadCuteFace();
        drawFace(0, "UNLOCKING");
        lcd.setCursor(6, 1);
        lcd.print(remaining);
        lcd.print("s ");
        for(int j = 0; j < (UNLOCK_COUNTDOWN - remaining); j++) {
          lcd.write(byte(4));
        }
      } else {
        // Unlock complete - go to sleeping
        phoneRemoved = true;
        checkingRemoval = false;
        unlockSequenceActive = false;
        challengeActive = false;
        inputIdx = 0;
        lcd.clear();
      }
    }
  } else {
    // Phone still in box - reset removal check
    checkingRemoval = false;
    unlockSequenceActive = false;
  }
}

void showSleepingAnimation(unsigned long currentTime) {
  if (currentTime - lastAnimationUpdate >= ANIMATION_FRAME_TIME) {
    animationFrame = (animationFrame + 1) % 4;
    lastAnimationUpdate = currentTime;
  }
  
  loadSleepingFace(animationFrame);
  
  int blinkState = 0;
  if (currentTime - lastBlink < 150) {
    blinkState = 1;
  } else if (currentTime - lastBlink >= BLINK_INTERVAL) {
    lastBlink = currentTime;
  }
  
  drawSleepingFace(blinkState, "ZZZ...");
}

void runLockingSequence(unsigned long currentTime) {
  unsigned long elapsed = currentTime - lockingSequenceStart;
  int remaining = LOCKING_COUNTDOWN - (elapsed / 1000);
  
  if (remaining > 0) {
    loadSurprisedFace();
    drawTwoRowFace((currentTime / 500) % 2, "LOCKING"); // Use two-row blink
    lcd.setCursor(6, 1);
    lcd.print(remaining);
    lcd.print("s ");
    for(int j = 0; j < (LOCKING_COUNTDOWN - remaining); j++) {
      lcd.write(byte(4));
    }
  } else {
    lockBox();
    lockingInProgress = false;
    phoneWasTakenAfterChallenge = false; // Reset after locking starts
  }
}

void showStudyingFace(unsigned long elapsed, unsigned long currentTime) {
  unsigned long remaining = STUDY_DURATION_MS - elapsed;
  loadFocusFace();
  
  int blinkState = 0;
  if (currentTime - lastBlink < 150) {
    blinkState = 1;
  } else if (currentTime - lastBlink >= BLINK_INTERVAL) {
    lastBlink = currentTime;
  }
  
  drawTwoRowFace(blinkState, "STUDYING"); // Use two-row blink
  showTime(remaining);
}

void showRandomCheatingExpression() {
  int exprType = random(5);
  String messages[5][2] = {
    {"NOT YET!!", "FOCUS!"},
    {"HEY!", "WHAT R U DOING?"},
    {"GO BACK", "TO WORK!"},
    {"SHY...", "NO CHEATING!"},
    {"DIZZY...", "STAY FOCUS!"}
  };
  
  switch(exprType) {
    case 0: loadAngryFace(); break;
    case 1: loadSuspiciousFace(); break;
    case 2: loadAngryFace(); break;
    case 3: loadShyFace(); break;
    case 4: loadDizzyFace(); break;
  }
  
  lcd.clear();
  drawTwoRowFace(0, messages[exprType][0]);
  lcd.setCursor(6, 1);
  lcd.print(messages[exprType][1]);
  showingAngry = true;
  angryFaceStart = millis(); // This was already here, keeping it
}

void handlePasswordChallenge(unsigned long currentTime, long dist) {
  if (!challengeActive) {
    // Show reward animation
    loadRewardFace();
    int animFrame = (currentTime / 400) % 3;
    drawTwoRowFace(animFrame % 2, "TIME UP!");
    lcd.setCursor(6, 1);
    lcd.print("GOOD JOB!");
    
    static unsigned long rewardStart = 0;
    if (rewardStart == 0) {
      rewardStart = currentTime;
      initialDistanceAtChallengeStart = dist;
      minDistanceDuringChallenge = dist;
    }
    
    if (currentTime - rewardStart < 3000) return;
    
    for(int i = 0; i < 3; i++) password[i] = random(1, 4);
    challengeActive = true;
    inputIdx = 0;
    rewardStart = 0;
    waitingForSleepAfterTimeUp = false;
    showingAngryInChallenge = false;
    lcd.clear();
  }
  
  // SIMPLIFIED: Track if phone is taken (distance increases significantly)
  if (dist < minDistanceDuringChallenge) {
    minDistanceDuringChallenge = dist;
  }
  
  // If phone is taken (distance increased significantly), start sleep countdown
  if (dist > minDistanceDuringChallenge + 5 && dist > DIST_PHONE_REMOVED) {
    if (!waitingForSleepAfterTimeUp) {
      waitingForSleepAfterTimeUp = true;
      phoneWasTakenAfterChallenge = true;
      unlockSequenceStart = currentTime;
      challengeActive = false;
      showingAngryInChallenge = false;
      lcd.clear();
    }
  }
  
  // If waiting for sleep countdown after phone taken
  if (waitingForSleepAfterTimeUp) {
    unsigned long elapsed = currentTime - unlockSequenceStart;
    int remaining = UNLOCK_COUNTDOWN - (elapsed / 1000);
    
    if (remaining > 0) {
      loadCuteFace();
      drawFace(0, "GOING SLEEP");
      lcd.setCursor(6, 1);
      lcd.print(remaining);
      lcd.print("s ");
      for(int j = 0; j < (UNLOCK_COUNTDOWN - remaining); j++) {
        lcd.write(byte(4));
      }
      return;
    } else {
      // Sleep countdown complete - go to sleeping, reset flags
      waitingForSleepAfterTimeUp = false;
      phoneRemoved = true;
      challengeActive = false;
      inputIdx = 0;
      phoneWasTakenAfterChallenge = true; // Keep this true so we know phone was taken
      lcd.clear();
      return;
    }
  }
  
  // Show angry expression if showing
  if (showingAngryInChallenge) {
    // Keep showing the angry face (it was already drawn)
    if (currentTime - angryFaceStartInChallenge >= ANGRY_DISPLAY_TIME) {
      showingAngryInChallenge = false;
      lcd.clear();
    }
    return; // Don't show code challenge while showing angry
  }
  
  // Check for cheating - ANY button press that's not correct code input
  if (btn1JustPressed || btn2JustPressed || btn3JustPressed) {
    bool isCorrectInput = false;
    if (inputIdx < 3) {
      int expected = password[inputIdx];
      if ((btn1JustPressed && expected == 1) || 
          (btn2JustPressed && expected == 2) || 
          (btn3JustPressed && expected == 3)) {
        isCorrectInput = true;
      }
    }
    
    if (!isCorrectInput) {
      // Show angry expression
      int exprType = random(5);
      String messages[5][2] = {
        {"NOT YET!!", "FOCUS!"},
        {"HEY!", "WHAT R U DOING?"},
        {"GO BACK", "TO WORK!"},
        {"SHY...", "NO CHEATING!"},
        {"DIZZY...", "STAY FOCUS!"}
      };
      
      switch(exprType) {
        case 0: loadAngryFace(); break;
        case 1: loadSuspiciousFace(); break;
        case 2: loadAngryFace(); break;
        case 3: loadShyFace(); break;
        case 4: loadDizzyFace(); break;
      }
      
      lcd.clear();
      drawTwoRowFace(0, messages[exprType][0]);
      lcd.setCursor(6, 1);
      lcd.print(messages[exprType][1]);
      showingAngryInChallenge = true;
      angryFaceStartInChallenge = currentTime;
      return; // Don't process as code input
    }
  }
  
  // Show code challenge
  loadHappyFace();
  drawFace(0, "CODE:" + String(password[0]) + String(password[1]) + String(password[2]));
  
  lcd.setCursor(6, 1);
  if (inputIdx == 0) lcd.print("___");
  else if (inputIdx == 1) lcd.print(String(inputSeq[0]) + "__");
  else if (inputIdx == 2) lcd.print(String(inputSeq[0]) + String(inputSeq[1]) + "_");
  else lcd.print(String(inputSeq[0]) + String(inputSeq[1]) + String(inputSeq[2]));
  
  // Process code input
  int b = 0;
  if (btn1JustPressed) b = 1;
  else if (btn2JustPressed) b = 2;
  else if (btn3JustPressed) b = 3;

  if (b > 0 && inputIdx < 3) {
    inputSeq[inputIdx++] = b;
    delay(300);
    if (inputIdx == 3) {
      if (inputSeq[0] == password[0] && inputSeq[1] == password[1] && inputSeq[2] == password[2]) {
        phoneWasTakenAfterChallenge = true; // ADDED THIS LINE
        unlockBox();
      } else {
        // Wrong code - show angry
        inputIdx = 0;
        int exprType = random(5);
        String messages[5][2] = {
          {"NOT YET!!", "FOCUS!"},
          {"HEY!", "WHAT R U DOING?"},
          {"GO BACK", "TO WORK!"},
          {"SHY...", "NO CHEATING!"},
          {"DIZZY...", "STAY FOCUS!"}
        };
        
        switch(exprType) {
          case 0: loadAngryFace(); break;
          case 1: loadSuspiciousFace(); break;
          case 2: loadAngryFace(); break;
          case 3: loadShyFace(); break;
          case 4: loadDizzyFace(); break;
        }
        
        lcd.clear();
        drawTwoRowFace(0, messages[exprType][0]);
        lcd.setCursor(6, 1);
        lcd.print(messages[exprType][1]);
        showingAngryInChallenge = true;
        angryFaceStartInChallenge = currentTime;
      }
    }
  }
}
// --- FACE DRAWING ---
// Two-row eyes (for locking/studying) - eyes split across two rows
void drawTwoRowFace(int blink, String txt) {
  lcd.clear();
  
  // Left Eye - split across two rows
  lcd.setCursor(0, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(0)); // Top
  lcd.setCursor(0, 1);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(1)); // Bottom
  
  // Right Eye - split across two rows
  lcd.setCursor(2, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(0)); // Top
  lcd.setCursor(2, 1);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(1)); // Bottom
  
  // Mouth centered
  lcd.setCursor(1, 1);
  lcd.write(byte(3));
  
  // Text
  lcd.setCursor(4, 0);
  lcd.print(txt);
}

// Single row eyes (for other states)
void drawFace(int blink, String txt) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(0));
  lcd.setCursor(2, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(1));
  lcd.setCursor(1, 1);
  lcd.write(byte(3));
  lcd.setCursor(4, 0);
  lcd.print(txt);
}

void drawSleepingFace(int blink, String txt) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(0));
  lcd.setCursor(2, 0);
  if(blink) lcd.write(byte(2));
  else lcd.write(byte(1));
  lcd.setCursor(1, 1);
  lcd.write(byte(3)); // Tiny dot mouth
  lcd.setCursor(4, 0);
  lcd.print(txt);
}

// --- FACE LOADERS ---
void loadSleepingFace(int frame) {
  if (frame == 0 || frame == 2) {
    lcd.createChar(0, sleepEye1);
    lcd.createChar(1, sleepEye1);
  } else if (frame == 1) {
    lcd.createChar(0, sleepEye2);
    lcd.createChar(1, sleepEye2);
  } else {
    lcd.createChar(0, sleepEye3);
    lcd.createChar(1, sleepEye3);
  }
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, tinyMouth); // Tiny dot mouth
}

void loadHappyFace() {
  lcd.createChar(0, eyeTop);
  lcd.createChar(1, eyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, happyMouth);
}

void loadCuteFace() {
  // Cuter face with smaller eyes and tiny mouth
  lcd.createChar(0, smallEye);
  lcd.createChar(1, smallEye);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, cuteMouth); // Tiny cute mouth
}

void loadFocusFace() {
  lcd.createChar(0, focusEyeTop);
  lcd.createChar(1, focusEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, neutralMouth);
  lcd.createChar(4, progressBar);
}

void loadAngryFace() {
  lcd.createChar(0, angryEyeTop);
  lcd.createChar(1, angryEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, angryMouth);
}

void loadSuspiciousFace() {
  lcd.createChar(0, suspiciousEyeTop);
  lcd.createChar(1, suspiciousEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, angryMouth);
}

void loadSurprisedFace() {
  lcd.createChar(0, surpriseEyeTop);
  lcd.createChar(1, surpriseEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, surpriseMouth);
  lcd.createChar(4, progressBar);
}

void loadRewardFace() {
  lcd.createChar(0, heartEyeTop);
  lcd.createChar(1, heartEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, rewardMouth);
}

void loadShyFace() {
  lcd.createChar(0, shyEyeTop);
  lcd.createChar(1, shyEyeBottom);
  lcd.createChar(2, eyeClosed);
  lcd.createChar(3, neutralMouth);
}

void loadDizzyFace() {
  lcd.createChar(0, xEyeTop);
  lcd.createChar(1, xEyeBottom);
  lcd.createChar(2, xEyeTop);
  lcd.createChar(3, neutralMouth);
}

// --- HELPERS ---
void lockBox() {
  myServo.attach(servoPin);
  myServo.write(180);
  delay(1000);
  myServo.detach();
  isLocked = true;
  lockStartTime = millis();
  challengeActive = false;
  showingAngry = false;
}

void unlockBox() {
  myServo.attach(servoPin);
  myServo.write(0);
  delay(1000);
  myServo.detach();
  isLocked = false;
  phoneRemoved = false; // CHANGED: Set to false to show removal prompt
  challengeActive = false;
  showingAngry = false;
  lockingInProgress = false;
  checkingRemoval = false;
  unlockSequenceActive = false;
  waitingForSleepAfterTimeUp = false;
  // Keep phoneWasTakenAfterChallenge as true to prevent immediate re-locking
  showingAngryInChallenge = false;
  inputIdx = 0;
  lcd.clear();
  lastAnimationUpdate = millis();
  lastBlink = millis();
}
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  if (duration == 0) return 999;
  return (duration / 2) / 29.1;
}

void showTime(unsigned long ms) {
  unsigned long sec = ms / 1000;
  unsigned long minutes = sec / 60;
  unsigned long seconds = sec % 60;
  if (minutes > 25) minutes = 25;
  lcd.setCursor(4, 1);
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print("m ");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("s   ");
}
