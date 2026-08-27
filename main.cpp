#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <SFML/Audio.hpp>
#include <vector>
#include <format>
#include <thread>
#include <chrono>
#include <future>
#include <iomanip>
#include <fstream>
#include <sstream>


/*
todo:
Добавить рендер экрана счета
*/

enum GameMode {
	START_MENU,
	GAME,
	COUNTDOWN,
	MENU,
	SCORE
};

enum class NoteType : uint8_t {
	Tap,
	Hold
};

struct Note {
	int32_t timeMs;
	uint8_t column;
	NoteType type;
	int32_t durationMs;
	bool isHit = false;
	bool isMissed = false;
};

struct Beatmap {
	sf::Music music;
	std::vector<Note> notes;

};



sf::Font font;



int score = 0;
float accuracy = 100.0f;
int notesTotal = 0;
int notesPassed = 0;

enum GameMode gameMode = START_MENU;



const int PERFECT_WINDOW = 20;
const int GOOD_WINDOW = 60;
const int MISS_WINDOW = 100;



sf::Clock countdownClock;
int countdownSeconds = 3;



void loadFont();
void startNewGame(Beatmap& beatmap);
NoteType parseNoteType(const std::string& str);
bool isThereNoMisses(Beatmap& beatmap);

int main() {





	unsigned  width = 500;
	unsigned  height = 800;
	sf::RenderWindow window(sf::VideoMode({width, height}), "VSRG");
	window.setFramerateLimit(100);






	Beatmap beatmap;






	if (!beatmap.music.openFromFile("Assets/Music/RN - Joey Valence & Brae.mp3"))
	{
		std::cerr << "Error loading music!\n";
		return -1;
	}
	beatmap.music.setVolume(5.f);





	loadFont();








	sf::Text countdown(font, "3", 100);
	countdown.setFillColor(sf::Color::Black);
	countdown.setPosition({width / 2.0f, height / 2.0f});

	sf::FloatRect countdownTextBounds = countdown.getLocalBounds();
	countdown.setOrigin({
		countdownTextBounds.position.x + countdownTextBounds.size.x / 2.0f,
		countdownTextBounds.position.y + countdownTextBounds.size.y / 2.0f
	});










	sf::Text welcomeText(font, "To start press ENTER", 67);
	welcomeText.setFillColor(sf::Color::Black);
	welcomeText.setOutlineThickness(2.f);
	welcomeText.setOutlineColor(sf::Color::White);
	welcomeText.setPosition({width / 2.0f, height / 2.0f});

	sf::FloatRect textBounds = welcomeText.getLocalBounds();
	welcomeText.setOrigin({
		textBounds.position.x + textBounds.size.x / 2.0f,
		textBounds.position.y + textBounds.size.y / 2.0f
	});










	sf::Text scoreText(font, "Score: 0", 42);
	scoreText.setFillColor(sf::Color::Black);
	scoreText.setOutlineThickness(2.f);
	scoreText.setOutlineColor(sf::Color::White);
	scoreText.setPosition({30.f, 0.f});

	sf::Text accuracyText(font, "Accuracy: 100.0%", 42);
	accuracyText.setFillColor(sf::Color::Black);
	accuracyText.setOutlineThickness(2.f);
	accuracyText.setOutlineColor(sf::Color::White);
	sf::FloatRect accuracyTextBounds = accuracyText.getLocalBounds();
	accuracyText.setPosition({width - accuracyTextBounds.size.x - 30, 0.f});





	sf::Text rankingText(font, ".", 100);
	rankingText.setFillColor(sf::Color::Black);
	rankingText.setOutlineThickness(2.f);
	rankingText.setOutlineColor(sf::Color::White);
	rankingText.setPosition({width / 2.f, height / 2.f});
	rankingText.setLineAlignment(sf::Text::LineAlignment::Center);








	sf::VertexArray backgroundGradient(sf::PrimitiveType::Triangles, 6);

	sf::Color backgroundTopColor = sf::Color(125, 125, 255);
	sf::Color backgroundBottomColor = sf::Color(8, 35, 176);


	backgroundGradient[0] = sf::Vertex({0.f, 0.f}, backgroundTopColor);
	backgroundGradient[1] = sf::Vertex({float(width), 0.f}, backgroundTopColor);
	backgroundGradient[2] = sf::Vertex({float(width), float(height)}, backgroundBottomColor);

	backgroundGradient[3] = sf::Vertex({0.f, 0.f}, backgroundTopColor);
	backgroundGradient[4] = sf::Vertex({float(width), float(height)}, backgroundBottomColor);
	backgroundGradient[5] = sf::Vertex({0.f, float(height)}, backgroundBottomColor);








	float columnPositions[4] = { 92.f, 201.f, 307.f, 415.f };









	float targetCircleRadius = 43.0f;
	float targetCircleHeight = 705.0f;

	sf::CircleShape targetCircles[4];
	for(int i = 0; i < 4; ++i){
		targetCircles[i].setRadius(targetCircleRadius);
		targetCircles[i].setOrigin({targetCircleRadius, targetCircleRadius});
		targetCircles[i].setFillColor(sf::Color::Transparent);
		targetCircles[i].setOutlineThickness(5.0f);
		targetCircles[i].setPosition({columnPositions[i], targetCircleHeight});
	}












	float scrollSpeed = 1.f;
	int visibilityWindowMs = 1200;









	float noteRadius = 40.f;
	sf::CircleShape noteVisual(targetCircleRadius);
	noteVisual.setOrigin({targetCircleRadius, targetCircleRadius});
	noteVisual.setFillColor(sf::Color(0, 0, 0, 128));









	std::string line;

	std::ifstream in("beatmaps/map.txt");
	if(in.is_open()){

		while (std::getline(in, line)) {

			std::stringstream ss(line);

			int32_t tempNoteTimeMs;
			int tempNoteColumn;
			std::string tempNoteType;
			int32_t tempNoteDuration;

			if(ss >> tempNoteTimeMs >> tempNoteColumn >> tempNoteType >> tempNoteDuration){

				Note tempNote;
				tempNote.timeMs = tempNoteTimeMs;
				tempNote.column = tempNoteColumn;
				tempNote.type = parseNoteType(tempNoteType);
				tempNote.durationMs = tempNoteDuration;

				beatmap.notes.push_back(tempNote);
			}
		}
	}
	else {
		std::cerr << "Can't read beatmap file!\n";
		return -1;
	}
	in.close();










	for (auto& note : beatmap.notes) {
		++notesTotal;
	}



	int lastNoteTimeInMs = beatmap.notes[beatmap.notes.size() - 1].timeMs;



	while(window.isOpen()){

		int currentSongTimeInMs = beatmap.music.getPlayingOffset().asMilliseconds();


		while(const std::optional event = window.pollEvent()) {



			if(event->is<sf::Event::Closed>()){
				window.close();
			}




			else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){

				if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					switch (gameMode) {
						case START_MENU:
							window.close();
							break;
						case GAME:
							gameMode = START_MENU;
							beatmap.music.stop();
							break;
						case COUNTDOWN:
							gameMode = START_MENU;
							break;
						case SCORE:
							gameMode = START_MENU;
							break;
					}
				}
				if (keyPressed->scancode == sf::Keyboard::Scancode::Z) {
					gameMode = SCORE;
					beatmap.music.stop();
				}




				if(gameMode == GAME){
					int targetColumn = -1;
					if(keyPressed->scancode == sf::Keyboard::Scancode::D) targetColumn = 0;
					if(keyPressed->scancode == sf::Keyboard::Scancode::F) targetColumn = 1;
					if(keyPressed->scancode == sf::Keyboard::Scancode::J) targetColumn = 2;
					if(keyPressed->scancode == sf::Keyboard::Scancode::K) targetColumn = 3;



					if (targetColumn != -1) {

						for (auto& note : beatmap.notes) {
							if (note.column == targetColumn && !note.isHit && !note.isMissed) {

								int timeDiff = std::abs(note.timeMs - currentSongTimeInMs);

								if (timeDiff <= MISS_WINDOW) {
									if (timeDiff <= PERFECT_WINDOW) {
										note.isHit = true;
										score += 300;
										std::cout << "Perfect" << '\n';
									}
									else if (timeDiff <= GOOD_WINDOW) {
										note.isHit = true;
										score += 200;
										std::cout << "good" << '\n';
									}
									else {
										note.isHit = true;
										score += 50;
										std::cout << "bad" << '\n';
									}
									++notesPassed;
									break;
								}
							}
						}
					}
				}
			}
		}




		if(notesPassed > 0) accuracy = (score / float(notesPassed * 300)) * 100;


		if (gameMode == GAME) {
			if (currentSongTimeInMs >= lastNoteTimeInMs) {
				gameMode = SCORE;
			}
		}

		if (gameMode == COUNTDOWN) {
			if (countdownClock.getElapsedTime().asSeconds() >= 1.0f) {
				countdownSeconds--;
				countdownClock.restart();

				if (countdownSeconds <= 0) {
					gameMode = GAME;
					beatmap.music.play();
				}
				else {
					countdown.setString(std::format("{}", countdownSeconds));
				}
			}
		}








		if (gameMode == START_MENU) {

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter)){
				startNewGame(beatmap);
				countdown.setString("3");
			}
		}










		for (int i = 0; i < 4; ++i) {
			targetCircles[i].setOutlineColor(sf::Color::Black);
		}



		if (gameMode == GAME || gameMode == COUNTDOWN) {

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)){
				targetCircles[0].setOutlineColor(sf::Color::Green);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::F)) {
				targetCircles[1].setOutlineColor(sf::Color::Green);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::J)) {
				targetCircles[2].setOutlineColor(sf::Color::Green);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::K)) {
				targetCircles[3].setOutlineColor(sf::Color::Green);
			}

		}










		//render
		window.clear();













		//drawing

		window.draw(backgroundGradient);





		switch (gameMode) {


			case START_MENU:

				window.draw(welcomeText);
				break;







			case GAME:

				scoreText.setString(std::format("Score: {}", score));
				accuracyText.setString(std::format("Accuracy: {}%", std::format("{:.0f}", accuracy)));



				for(int i = 0; i < 4; ++i){
					window.draw(targetCircles[i]);
				}



				for (auto& note : beatmap.notes) {

					int timeRemaining = note.timeMs - currentSongTimeInMs;

					if(!note.isHit){


						if (timeRemaining <= visibilityWindowMs && timeRemaining >= -MISS_WINDOW) {
							float noteY = targetCircleHeight - (scrollSpeed * timeRemaining);
							noteVisual.setPosition({columnPositions[note.column], noteY});
							window.draw(noteVisual);
						}
					}




					if (!note.isHit && !note.isMissed) {
						if (timeRemaining < -MISS_WINDOW) {
							note.isMissed = true;
							++notesPassed;
							std::cout << "Miss" << '\n';
						}
					}




				}




				window.draw(accuracyText);
				window.draw(scoreText);

				break;








			case COUNTDOWN:

				scoreText.setString(std::format("Score: {}", score));
				accuracyText.setString(std::format("Accuracy: {}%", std::format("{:.0f}", accuracy)));

				for(int i = 0; i < 4; ++i){
					window.draw(targetCircles[i]);
				}
				window.draw(accuracyText);
				window.draw(scoreText);
				window.draw(countdown);


				break;





			case SCORE:

				if (accuracy  == 100.f) {
					rankingText.setString(std::format("SS\n{}%", accuracy));
				}
				else if (accuracy >= 90.f && isThereNoMisses(beatmap)) {
					rankingText.setString(std::format("S\n{}%", accuracy));
				}
				else if (accuracy >= 90.f  || (accuracy >= 80.f && isThereNoMisses(beatmap))) {
					rankingText.setString(std::format("A\n{}%", accuracy));
				}
				else if (accuracy >= 80.f || (accuracy >= 70.f && isThereNoMisses(beatmap))) {
					rankingText.setString(std::format("B\n{}%", accuracy));
				}
				else if (accuracy >= 60.f) {
					rankingText.setString(std::format("C\n{}%", accuracy));
				}
				else {
					rankingText.setString(std::format("D\n{}%", accuracy));
				}

				sf::FloatRect rankingTextBounds = rankingText.getLocalBounds();
				rankingText.setOrigin({
					rankingTextBounds.position.x + rankingTextBounds.size.x / 2.0f,
					rankingTextBounds.position.y + rankingTextBounds.size.y / 2.0f
				});

				window.draw(rankingText);

				break;
		}









		window.display();
	}


	return 0;
}







void loadFont(){
	if (!font.openFromFile("Assets/Font/PB Pixel.ttf")) {
		std::cerr << "Error loading font!\n";
		std::exit(EXIT_FAILURE);
	}
}






void startNewGame(Beatmap& beatmap){
	for(auto& note : beatmap.notes) { note.isHit = false; note.isMissed = false; }
	score = 0;
	accuracy = 100.0f;

	gameMode = COUNTDOWN;
	countdownSeconds = 3;
	countdownClock.restart();


}






NoteType parseNoteType(const std::string& str) {

	if (str == "NoteType::Tap") {
		return NoteType::Tap;
	}

	if (str == "NoteType::Hold") {
		return NoteType::Hold;
	}


	return NoteType::Tap;
}







bool isThereNoMisses(Beatmap& beatmap){
	for (auto note : beatmap.notes) {
		if (note.isMissed) {
			return false;
		}
	}
	return true;
}



