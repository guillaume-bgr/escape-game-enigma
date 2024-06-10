#include <Arduino_MKRIoTCarrier.h>
#include <iomanip>
MKRIoTCarrier carrier;
float x, y, z;
bool timerStarted = false;
bool isFinished = false;
int stepQuiz = 0;
const int timer = 60250;
int timeLeft = timer;
int answers[4];
int touched[5] = {false, false, false, false, false};
char formattedNumber[3];

struct Step {
    String question[2];
    int options[5];
    int answer[4];
};

Step steps[] = {
	{
		{"? x (? - ?) x ?", "\n = 90" },
		{ 4, 13, 11, 5, 2 },
		{ 5, 11, 4, 2 },
	},
	{
		{"? + ? / ?", "\n = 18" },
		{ 12, 3, 16, 15, 6 },
		{ 16, 12, 6 },
	},
	{
		{"? x (? - ?) x ?", "\n = 174" },
		{ 3, 16, -2, 45, 6 },
		{ 3, 16, 45, -2 },
	},
};

void setup() {
	carrier.withCase();
	carrier.begin();
	carrier.display.setTextSize(8);
	carrier.display.setTextColor(0xFFFF);
	carrier.leds.setBrightness(10);
}

void loop() {
	if (isFinished) {
		youWin();
		delay(3000);
	} else {
		carrier.Buttons.update();
		if (carrier.IMUmodule.gyroscopeAvailable()) {
			carrier.IMUmodule.readGyroscope(x, y, z);
			if (abs(y) > 40 || abs(y) > 40) {
				if (timerStarted != true) {
					getQuiz(stepQuiz);
					timerStarted = true;
				}
			}
		}
		if (timerStarted) {
			if (timeLeft > 1) {
				checkQuiz(stepQuiz);
				decrementTimer();
			} else {
				boom();
				delay(1000);
				resetTimer();
			}
		}
		beep();
		delay(250);
	}
}

void checkQuiz(int step) {
	bool answerGiven = false;
	if (carrier.Buttons.getTouch(TOUCH0)) {
		if (touched[0] == false) {
			answers[getNextAnswerNb()] = steps[step].options[0];
			answerGiven = true;
			touched[0] = true;
		}
	}			
	if (carrier.Buttons.getTouch(TOUCH1)) {
		if (touched[1] == false) {
			answers[getNextAnswerNb()] = steps[step].options[1];
			answerGiven = true;
			touched[1] = true;
		}
	}
	if (carrier.Buttons.getTouch(TOUCH2)) {
		if (touched[2] == false) {
			answers[getNextAnswerNb()] = steps[step].options[2];
			answerGiven = true;
			touched[2] = true;
		}
	}
	if (carrier.Buttons.getTouch(TOUCH3)) {
		if (touched[3] == false) {
			answers[getNextAnswerNb()] = steps[step].options[3];
			answerGiven = true;
			touched[3] = true;
		}
	}
	if (carrier.Buttons.getTouch(TOUCH4)) {
		if (touched[4] == false) {
			answers[getNextAnswerNb()] = steps[step].options[4];
			answerGiven = true;
			touched[4] = true;
		}
	}
	if (answerGiven) {
		if (!validateAnswers(step)) {
			delay(200);
			getQuiz(step);
		}
	}	
}

bool validateAnswers(int step) {
	carrier.leds.clear();
	for (int i = 0; i < 4; i++) {
		int optionIndex = getAnswerOptNb(step, answers[i]); 
		if (answers[i] == steps[step].answer[i]) {
			carrier.leds.fill(carrier.leds.Color(0 ,255 ,0), optionIndex, 1);
			carrier.leds.show();
		} else {
			if (answers[i] != 0) {
				carrier.leds.fill(carrier.leds.Color(255, 0, 0), optionIndex,1);
				carrier.leds.show();
				return false;
			} else {
				return true;
			}
		}
	}
	stepQuiz = stepQuiz + 1;
	isFinished = !getQuiz(stepQuiz);
}

int getAnswerOptNb(int step, int answer) {
	for (int i = 0; i < 5; i++) {
		if (answer == steps[step].options[i]) {
			return i;
		}
	}
}

void resetTimer() {
	carrier.display.fillScreen(0x0000);
	carrier.display.setTextSize(8);
	for (int x = 0; x < sizeof(answers) / sizeof(answers[0]); x++) {
		answers[x] = 0;
	}
	for (int x = 0; x < sizeof(touched) / sizeof(touched[0]); x++) {
		touched[x] = false;
	}
	stepQuiz = 0;
	timeLeft = timer;
	timerStarted = false;
}

void decrementTimer() {
	timeLeft = timeLeft - 250;
	if (timeLeft % 1000 == 0) {
		carrier.display.setTextSize(8);
		carrier.display.setCursor(140, 10);
		carrier.display.fillRect(140, 10, 100, 60, 0x0000);
		sprintf(formattedNumber, "%02d", timeLeft / 1000);
		carrier.display.print(formattedNumber);
	}
}

void boom() {
	carrier.display.fillScreen(0x0000);
	carrier.display.setTextSize(9);
	carrier.display.setTextColor(0xF800);
	carrier.display.setCursor(10, 60);
	carrier.display.print("BOOM");
	carrier.display.setTextColor(0xFFFF);
}

void beep() {
	if (timerStarted) {
		if (timeLeft > 40000 && timeLeft % 2000 == 0) {
			carrier.Buzzer.beep(1600, 20);
		} else if (timeLeft <= 40000 && timeLeft % 1500 == 0) {
			carrier.Buzzer.beep(1600, 20);
		} else if (timeLeft <= 30000 && timeLeft % 1000 == 0) {
			carrier.Buzzer.beep(1600, 20);
		} else if (timeLeft <= 15000 && timeLeft % 500 == 0) {
			carrier.Buzzer.beep(1600, 20);
		} else if (timeLeft <= 7500 && timeLeft % 250 == 0) {
			carrier.Buzzer.beep(1600, 20);
		}
	}
}

boolean getQuiz(int step) {
	for (int x = 0; x < sizeof(answers) / sizeof(answers[0]); x++) {
		answers[x] = 0;
	}
	for (int x = 0; x < sizeof(touched) / sizeof(touched[0]); x++) {
		touched[x] = false;
	}
    int totalSteps = sizeof(steps) / sizeof(steps[0]);
    if (step >= 0 && step < totalSteps) {
      	Serial.println("Step exists!");
    } else {
      	return false;
    }
    // Question
    carrier.display.fillRect(0, 70, 240, 240, 0x0000);
    carrier.display.setCursor(10, 100);
    carrier.display.setTextSize(3);
    carrier.display.print(steps[step].question[0]);
    carrier.display.setTextColor(0x2469);
    carrier.display.print(steps[step].question[1]);
    carrier.display.setTextColor(0xFFFF);
    // Choices
    carrier.display.setTextSize(2);
    carrier.display.setCursor(10, 220);
    carrier.display.print(String(steps[step].options[0]));
    carrier.display.setCursor(70, 220);
    carrier.display.print(String(steps[step].options[1]));
    carrier.display.setCursor(110, 220);
    carrier.display.print(String(steps[step].options[2]));
    carrier.display.setCursor(150, 220);
    carrier.display.print(String(steps[step].options[3]));
    carrier.display.setCursor(210, 220);
    carrier.display.print(String(steps[step].options[4]));
    carrier.leds.fill(carrier.leds.Color(255,255,255), 0, 5);
    carrier.leds.show();

	return true;
}

void youWin() {
	carrier.display.fillScreen(0x0000);
	carrier.display.setTextSize(4);
	carrier.display.setTextColor(0x07E0);
	carrier.display.setCursor(10, 70);
	carrier.display.print("YOU WIN !");
	carrier.display.setTextColor(0xFFE0);
}

int getNextAnswerNb() {
	for (int i = 0; i < 5; i++) {
		if (answers[i] == 0) {
			return i;
		}
	}
}
