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


/*
todo:
Поменять цвет нот
сделать чтение битмапы с файла
сделать выход по концу песни
сделать индикатор длительности трека

*/

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
int mode = 0;



const int PERFECT_WINDOW = 20;
const int GOOD_WINDOW = 60;
const int MISS_WINDOW = 100;



sf::Clock countdownClock;
int countdownSeconds = 3;



void loadFont();
void startNewGame(Beatmap& beatmap);

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
	welcomeText.setPosition({width / 2.0f, height / 2.0f});

	sf::FloatRect textBounds = welcomeText.getLocalBounds();
	welcomeText.setOrigin({
		textBounds.position.x + textBounds.size.x / 2.0f,
		textBounds.position.y + textBounds.size.y / 2.0f
	});










	sf::Text scoreText(font, "Score: 0", 42);
	scoreText.setFillColor(sf::Color::Black);
	scoreText.setPosition({30.f, 0.f});

	sf::Text accuracyText(font, "Accuracy: 100.0%", 42);
	accuracyText.setFillColor(sf::Color::Black);
	sf::FloatRect accuracyTextBounds = accuracyText.getLocalBounds();
	accuracyText.setPosition({width - accuracyTextBounds.size.x - 30, 0.f});






	sf::VertexArray backgroundGradient(sf::PrimitiveType::Triangles, 6);


	backgroundGradient[0] = sf::Vertex({0.f, 0.f}, sf::Color(125, 125, 255));
	backgroundGradient[1] = sf::Vertex({500.f, 0.f}, sf::Color(125, 125, 255));
	backgroundGradient[2] = sf::Vertex({500.f, 800.f}, sf::Color(8, 35, 176));

	backgroundGradient[3] = sf::Vertex({0.f, 0.f}, sf::Color(125, 125, 255));
	backgroundGradient[4] = sf::Vertex({500.f, 800.f}, sf::Color(8, 35, 176));
	backgroundGradient[5] = sf::Vertex({0.f, 800.f}, sf::Color(8, 35, 176));








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












	float scrollSpeed = 1.2f;
	int visibilityWindowMs = 1200;









	float noteRadius = 40.f;
	sf::CircleShape noteVisual(targetCircleRadius);
	noteVisual.setOrigin({targetCircleRadius, targetCircleRadius});
	noteVisual.setFillColor(sf::Color(0, 0, 0, 128));













	beatmap.notes = {
		{ 1200, 0, NoteType::Tap, 0 }, { 1800, 3, NoteType::Tap, 0 }, { 2400, 1, NoteType::Tap, 0 }, { 3000, 2, NoteType::Tap, 0 },
		{ 3600, 0, NoteType::Tap, 0 }, { 4200, 3, NoteType::Tap, 0 }, { 4800, 1, NoteType::Tap, 0 }, { 5100, 2, NoteType::Tap, 0 },
		{ 5400, 2, NoteType::Tap, 0 }, { 6000, 0, NoteType::Tap, 0 }, { 6000, 3, NoteType::Tap, 0 }, { 6600, 1, NoteType::Tap, 0 },
		{ 6600, 2, NoteType::Tap, 0 }, { 7200, 0, NoteType::Tap, 0 }, { 7500, 1, NoteType::Tap, 0 }, { 7800, 2, NoteType::Tap, 0 },
		{ 8100, 3, NoteType::Tap, 0 }, { 8400, 0, NoteType::Tap, 0 }, { 8550, 1, NoteType::Tap, 0 }, { 8700, 2, NoteType::Tap, 0 },
		{ 8850, 3, NoteType::Tap, 0 }, { 9000, 0, NoteType::Tap, 0 }, { 9000, 3, NoteType::Tap, 0 }, { 9600, 1, NoteType::Tap, 0 },
		{ 9600, 2, NoteType::Tap, 0 }, { 10200, 0, NoteType::Tap, 0 }, { 10800, 3, NoteType::Tap, 0 }, { 11400, 1, NoteType::Tap, 0 },
		{ 11700, 2, NoteType::Tap, 0 }, { 12000, 0, NoteType::Tap, 0 }, { 12000, 3, NoteType::Tap, 0 }, { 12600, 1, NoteType::Tap, 0 },
		{ 12900, 2, NoteType::Tap, 0 }, { 13200, 1, NoteType::Tap, 0 }, { 13500, 2, NoteType::Tap, 0 }, { 13800, 0, NoteType::Tap, 0 },
		{ 14400, 0, NoteType::Tap, 0 }, { 14400, 3, NoteType::Tap, 0 }, { 15000, 1, NoteType::Tap, 0 }, { 15000, 2, NoteType::Tap, 0 },
		{ 15600, 0, NoteType::Tap, 0 }, { 15750, 1, NoteType::Tap, 0 }, { 15900, 2, NoteType::Tap, 0 }, { 16050, 3, NoteType::Tap, 0 },
		{ 16200, 0, NoteType::Tap, 0 }, { 16200, 3, NoteType::Tap, 0 }, { 16800, 1, NoteType::Tap, 0 }, { 16950, 2, NoteType::Tap, 0 },
		{ 17100, 1, NoteType::Tap, 0 }, { 17400, 3, NoteType::Tap, 0 }, { 18000, 0, NoteType::Tap, 0 }, { 18000, 2, NoteType::Tap, 0 },
		{ 18600, 1, NoteType::Tap, 0 }, { 18600, 3, NoteType::Tap, 0 }, { 19200, 0, NoteType::Tap, 0 }, { 19500, 1, NoteType::Tap, 0 },
		{ 19800, 2, NoteType::Tap, 0 }, { 20100, 3, NoteType::Tap, 0 }, { 20400, 0, NoteType::Tap, 0 }, { 20400, 1, NoteType::Tap, 0 },
		{ 21000, 2, NoteType::Tap, 0 }, { 21000, 3, NoteType::Tap, 0 }, { 21600, 0, NoteType::Tap, 0 }, { 21750, 1, NoteType::Tap, 0 },
		{ 21900, 2, NoteType::Tap, 0 }, { 22050, 3, NoteType::Tap, 0 }, { 22200, 1, NoteType::Tap, 0 }, { 22200, 2, NoteType::Tap, 0 },
		{ 22800, 0, NoteType::Tap, 0 }, { 22950, 1, NoteType::Tap, 0 }, { 23100, 2, NoteType::Tap, 0 }, { 23400, 3, NoteType::Tap, 0 },
		{ 24000, 0, NoteType::Tap, 0 }, { 24000, 3, NoteType::Tap, 0 }, { 24600, 1, NoteType::Tap, 0 }, { 24600, 2, NoteType::Tap, 0 },
		{ 25200, 0, NoteType::Tap, 0 }, { 25500, 1, NoteType::Tap, 0 }, { 25800, 2, NoteType::Tap, 0 }, { 26100, 3, NoteType::Tap, 0 },
		{ 26400, 0, NoteType::Tap, 0 }, { 26550, 1, NoteType::Tap, 0 }, { 26700, 2, NoteType::Tap, 0 }, { 26850, 3, NoteType::Tap, 0 },
		{ 27000, 0, NoteType::Tap, 0 }, { 27000, 3, NoteType::Tap, 0 }, { 27600, 1, NoteType::Tap, 0 }, { 27600, 2, NoteType::Tap, 0 },
		{ 28200, 0, NoteType::Tap, 0 }, { 28800, 3, NoteType::Tap, 0 }, { 29400, 1, NoteType::Tap, 0 }, { 29700, 2, NoteType::Tap, 0 },
		{ 30000, 0, NoteType::Tap, 0 }, { 30000, 3, NoteType::Tap, 0 }, { 30600, 1, NoteType::Tap, 0 }, { 30900, 2, NoteType::Tap, 0 },
		{ 31200, 1, NoteType::Tap, 0 }, { 31500, 2, NoteType::Tap, 0 }, { 31800, 0, NoteType::Tap, 0 }, { 32400, 0, NoteType::Tap, 0 },
		{ 32400, 3, NoteType::Tap, 0 }, { 33000, 1, NoteType::Tap, 0 }, { 33000, 2, NoteType::Tap, 0 }, { 33600, 0, NoteType::Tap, 0 },
		{ 33750, 1, NoteType::Tap, 0 }, { 33900, 2, NoteType::Tap, 0 }, { 34050, 3, NoteType::Tap, 0 }, { 34200, 0, NoteType::Tap, 0 },
		{ 34200, 3, NoteType::Tap, 0 }, { 34800, 1, NoteType::Tap, 0 }, { 34950, 2, NoteType::Tap, 0 }, { 35100, 1, NoteType::Tap, 0 },
		{ 35400, 3, NoteType::Tap, 0 }, { 36000, 0, NoteType::Tap, 0 }, { 36000, 2, NoteType::Tap, 0 }, { 36600, 1, NoteType::Tap, 0 },
		{ 36600, 3, NoteType::Tap, 0 }, { 37200, 0, NoteType::Tap, 0 }, { 37500, 1, NoteType::Tap, 0 }, { 37800, 2, NoteType::Tap, 0 },
		{ 38100, 3, NoteType::Tap, 0 }, { 38400, 0, NoteType::Tap, 0 }, { 38400, 1, NoteType::Tap, 0 }, { 39000, 2, NoteType::Tap, 0 },
		{ 39000, 3, NoteType::Tap, 0 }, { 39600, 0, NoteType::Tap, 0 }, { 39750, 1, NoteType::Tap, 0 }, { 39900, 2, NoteType::Tap, 0 },
		{ 40050, 3, NoteType::Tap, 0 }, { 40200, 1, NoteType::Tap, 0 }, { 40200, 2, NoteType::Tap, 0 }, { 40800, 0, NoteType::Tap, 0 },
		{ 40950, 1, NoteType::Tap, 0 }, { 41100, 2, NoteType::Tap, 0 }, { 41400, 3, NoteType::Tap, 0 }, { 42000, 0, NoteType::Tap, 0 },
		{ 42000, 3, NoteType::Tap, 0 }, { 42600, 1, NoteType::Tap, 0 }, { 42600, 2, NoteType::Tap, 0 }, { 43200, 0, NoteType::Tap, 0 },
		{ 43500, 1, NoteType::Tap, 0 }, { 43800, 2, NoteType::Tap, 0 }, { 44100, 3, NoteType::Tap, 0 }, { 44400, 0, NoteType::Tap, 0 },
		{ 44550, 1, NoteType::Tap, 0 }, { 44700, 2, NoteType::Tap, 0 }, { 44850, 3, NoteType::Tap, 0 }, { 45000, 0, NoteType::Tap, 0 },
		{ 45000, 3, NoteType::Tap, 0 }, { 45600, 1, NoteType::Tap, 0 }, { 45600, 2, NoteType::Tap, 0 }, { 46200, 0, NoteType::Tap, 0 },
		{ 46800, 3, NoteType::Tap, 0 }, { 47400, 1, NoteType::Tap, 0 }, { 47700, 2, NoteType::Tap, 0 }, { 48000, 0, NoteType::Tap, 0 },
		{ 48000, 3, NoteType::Tap, 0 }, { 48600, 1, NoteType::Tap, 0 }, { 48900, 2, NoteType::Tap, 0 }, { 49200, 1, NoteType::Tap, 0 },
		{ 49500, 2, NoteType::Tap, 0 }, { 49800, 0, NoteType::Tap, 0 }, { 50400, 0, NoteType::Tap, 0 }, { 50400, 3, NoteType::Tap, 0 },
		{ 51000, 1, NoteType::Tap, 0 }, { 51000, 2, NoteType::Tap, 0 }, { 51600, 0, NoteType::Tap, 0 }, { 51750, 1, NoteType::Tap, 0 },
		{ 51900, 2, NoteType::Tap, 0 }, { 52050, 3, NoteType::Tap, 0 }, { 52200, 0, NoteType::Tap, 0 }, { 52200, 3, NoteType::Tap, 0 },
		{ 52800, 1, NoteType::Tap, 0 }, { 52950, 2, NoteType::Tap, 0 }, { 53100, 1, NoteType::Tap, 0 }, { 53400, 3, NoteType::Tap, 0 },
		{ 54000, 0, NoteType::Tap, 0 }, { 54000, 2, NoteType::Tap, 0 }, { 54600, 1, NoteType::Tap, 0 }, { 54600, 3, NoteType::Tap, 0 },
		{ 55200, 0, NoteType::Tap, 0 }, { 55500, 1, NoteType::Tap, 0 }, { 55800, 2, NoteType::Tap, 0 }, { 56100, 3, NoteType::Tap, 0 },
		{ 56400, 0, NoteType::Tap, 0 }, { 56400, 1, NoteType::Tap, 0 }, { 57000, 2, NoteType::Tap, 0 }, { 57000, 3, NoteType::Tap, 0 },
		{ 57600, 0, NoteType::Tap, 0 }, { 57750, 1, NoteType::Tap, 0 }, { 57900, 2, NoteType::Tap, 0 }, { 58050, 3, NoteType::Tap, 0 },
		{ 58200, 1, NoteType::Tap, 0 }, { 58200, 2, NoteType::Tap, 0 }, { 58800, 0, NoteType::Tap, 0 }, { 58950, 1, NoteType::Tap, 0 },
		{ 59100, 2, NoteType::Tap, 0 }, { 59400, 3, NoteType::Tap, 0 }, { 60000, 0, NoteType::Tap, 0 }, { 60000, 3, NoteType::Tap, 0 },
		{ 60600, 1, NoteType::Tap, 0 }, { 60600, 2, NoteType::Tap, 0 }, { 61200, 0, NoteType::Tap, 0 }, { 61500, 1, NoteType::Tap, 0 },
		{ 61800, 2, NoteType::Tap, 0 }, { 62100, 3, NoteType::Tap, 0 }, { 62400, 0, NoteType::Tap, 0 }, { 62550, 1, NoteType::Tap, 0 },
		{ 62700, 2, NoteType::Tap, 0 }, { 62850, 3, NoteType::Tap, 0 }, { 63000, 0, NoteType::Tap, 0 }, { 63000, 3, NoteType::Tap, 0 },
		{ 63600, 1, NoteType::Tap, 0 }, { 63600, 2, NoteType::Tap, 0 }, { 64200, 0, NoteType::Tap, 0 }, { 64800, 3, NoteType::Tap, 0 },
		{ 65400, 1, NoteType::Tap, 0 }, { 65700, 2, NoteType::Tap, 0 }, { 66000, 0, NoteType::Tap, 0 }, { 66000, 3, NoteType::Tap, 0 },
		{ 66600, 1, NoteType::Tap, 0 }, { 66900, 2, NoteType::Tap, 0 }, { 67200, 1, NoteType::Tap, 0 }, { 67500, 2, NoteType::Tap, 0 },
		{ 67800, 0, NoteType::Tap, 0 }, { 68400, 0, NoteType::Tap, 0 }, { 68400, 3, NoteType::Tap, 0 }, { 69000, 1, NoteType::Tap, 0 },
		{ 69000, 2, NoteType::Tap, 0 }, { 69600, 0, NoteType::Tap, 0 }, { 69750, 1, NoteType::Tap, 0 }, { 69900, 2, NoteType::Tap, 0 },
		{ 70050, 3, NoteType::Tap, 0 }, { 70200, 0, NoteType::Tap, 0 }, { 70200, 3, NoteType::Tap, 0 }, { 70800, 1, NoteType::Tap, 0 },
		{ 70950, 2, NoteType::Tap, 0 }, { 71100, 1, NoteType::Tap, 0 }, { 71400, 3, NoteType::Tap, 0 }, { 72000, 0, NoteType::Tap, 0 },
		{ 72000, 2, NoteType::Tap, 0 }, { 72600, 1, NoteType::Tap, 0 }, { 72600, 3, NoteType::Tap, 0 }, { 73200, 0, NoteType::Tap, 0 },
		{ 73500, 1, NoteType::Tap, 0 }, { 73800, 2, NoteType::Tap, 0 }, { 74100, 3, NoteType::Tap, 0 }, { 74400, 0, NoteType::Tap, 0 },
		{ 74400, 1, NoteType::Tap, 0 }, { 75000, 2, NoteType::Tap, 0 }, { 75000, 3, NoteType::Tap, 0 }, { 75600, 0, NoteType::Tap, 0 },
		{ 75750, 1, NoteType::Tap, 0 }, { 75900, 2, NoteType::Tap, 0 }, { 76050, 3, NoteType::Tap, 0 }, { 76200, 1, NoteType::Tap, 0 },
		{ 76200, 2, NoteType::Tap, 0 }, { 76800, 0, NoteType::Tap, 0 }, { 76950, 1, NoteType::Tap, 0 }, { 77100, 2, NoteType::Tap, 0 },
		{ 77400, 3, NoteType::Tap, 0 }, { 78000, 0, NoteType::Tap, 0 }, { 78000, 3, NoteType::Tap, 0 }, { 78600, 1, NoteType::Tap, 0 },
		{ 78600, 2, NoteType::Tap, 0 }, { 79200, 0, NoteType::Tap, 0 }, { 79500, 1, NoteType::Tap, 0 }, { 79800, 2, NoteType::Tap, 0 },
		{ 80100, 3, NoteType::Tap, 0 }, { 80400, 0, NoteType::Tap, 0 }, { 80550, 1, NoteType::Tap, 0 }, { 80700, 2, NoteType::Tap, 0 },
		{ 80850, 3, NoteType::Tap, 0 }, { 81000, 0, NoteType::Tap, 0 }, { 81000, 3, NoteType::Tap, 0 }, { 81600, 1, NoteType::Tap, 0 },
		{ 81600, 2, NoteType::Tap, 0 }, { 82200, 0, NoteType::Tap, 0 }, { 82800, 3, NoteType::Tap, 0 }, { 83400, 1, NoteType::Tap, 0 },
		{ 83700, 2, NoteType::Tap, 0 }, { 84000, 0, NoteType::Tap, 0 }, { 84000, 3, NoteType::Tap, 0 }, { 84600, 1, NoteType::Tap, 0 },
		{ 84900, 2, NoteType::Tap, 0 }, { 85200, 1, NoteType::Tap, 0 }, { 85500, 2, NoteType::Tap, 0 }, { 85800, 0, NoteType::Tap, 0 },
		{ 86400, 0, NoteType::Tap, 0 }, { 86400, 3, NoteType::Tap, 0 }, { 87000, 1, NoteType::Tap, 0 }, { 87000, 2, NoteType::Tap, 0 },
		{ 87600, 0, NoteType::Tap, 0 }, { 87750, 1, NoteType::Tap, 0 }, { 87900, 2, NoteType::Tap, 0 }, { 88050, 3, NoteType::Tap, 0 },
		{ 88200, 0, NoteType::Tap, 0 }, { 88200, 3, NoteType::Tap, 0 }, { 88800, 1, NoteType::Tap, 0 }, { 88950, 2, NoteType::Tap, 0 },
		{ 89100, 1, NoteType::Tap, 0 }, { 89400, 3, NoteType::Tap, 0 }, { 90000, 0, NoteType::Tap, 0 }, { 90000, 2, NoteType::Tap, 0 },{ 90600, 1, NoteType::Tap, 0 }, { 90600, 3, NoteType::Tap, 0 }, { 91200, 0, NoteType::Tap, 0 }, { 91500, 1, NoteType::Tap, 0 },{ 91800, 2, NoteType::Tap, 0 }, { 92100, 3, NoteType::Tap, 0 }, { 92400, 0, NoteType::Tap, 0 }, { 92400, 1, NoteType::Tap, 0 },{ 93000, 2, NoteType::Tap, 0 }, { 93000, 3, NoteType::Tap, 0 }, { 93600, 0, NoteType::Tap, 0 }, { 93750, 1, NoteType::Tap, 0 },{ 93900, 2, NoteType::Tap, 0 }, { 94050, 3, NoteType::Tap, 0 }, { 94200, 1, NoteType::Tap, 0 }, { 94200, 2, NoteType::Tap, 0 },{ 94800, 0, NoteType::Tap, 0 }, { 94950, 1, NoteType::Tap, 0 }, { 95100, 2, NoteType::Tap, 0 }, { 95400, 3, NoteType::Tap, 0 },{ 96000, 0, NoteType::Tap, 0 }, { 96000, 3, NoteType::Tap, 0 }, { 96600, 1, NoteType::Tap, 0 }, { 96600, 2, NoteType::Tap, 0 },{ 97200, 0, NoteType::Tap, 0 }, { 97500, 1, NoteType::Tap, 0 }, { 97800, 2, NoteType::Tap, 0 }, { 98100, 3, NoteType::Tap, 0 },{ 98400, 0, NoteType::Tap, 0 }, { 98550, 1, NoteType::Tap, 0 }, { 98700, 2, NoteType::Tap, 0 }, { 98850, 3, NoteType::Tap, 0 },{ 99000, 0, NoteType::Tap, 0 }, { 99000, 3, NoteType::Tap, 0 }, { 99600, 1, NoteType::Tap, 0 }, { 99600, 2, NoteType::Tap, 0 },{ 100200, 0, NoteType::Tap, 0 }, { 100800, 3, NoteType::Tap, 0 }, { 101400, 1, NoteType::Tap, 0 }, { 101700, 2, NoteType::Tap, 0 },{ 102000, 0, NoteType::Tap, 0 }, { 102000, 3, NoteType::Tap, 0 }, { 102600, 1, NoteType::Tap, 0 }, { 102900, 2, NoteType::Tap, 0 },{ 103200, 1, NoteType::Tap, 0 }, { 103500, 2, NoteType::Tap, 0 }, { 103800, 0, NoteType::Tap, 0 }, { 104400, 0, NoteType::Tap, 0 },{ 104400, 3, NoteType::Tap, 0 }, { 105000, 1, NoteType::Tap, 0 }, { 105000, 2, NoteType::Tap, 0 }, { 105600, 0, NoteType::Tap, 0 },{ 105750, 1, NoteType::Tap, 0 }, { 105900, 2, NoteType::Tap, 0 }, { 106050, 3, NoteType::Tap, 0 }, { 106200, 0, NoteType::Tap, 0 },{ 106200, 3, NoteType::Tap, 0 }, { 106800, 1, NoteType::Tap, 0 }, { 106950, 2, NoteType::Tap, 0 }, { 107100, 1, NoteType::Tap, 0 },{ 107400, 3, NoteType::Tap, 0 }, { 108000, 0, NoteType::Tap, 0 }, { 108000, 2, NoteType::Tap, 0 }, { 108600, 1, NoteType::Tap, 0 },{ 108600, 3, NoteType::Tap, 0 }, { 109200, 0, NoteType::Tap, 0 }, { 109500, 1, NoteType::Tap, 0 }, { 109800, 2, NoteType::Tap, 0 },{ 110100, 3, NoteType::Tap, 0 }, { 110400, 0, NoteType::Tap, 0 }, { 110400, 1, NoteType::Tap, 0 }, { 111000, 2, NoteType::Tap, 0 },{ 111000, 3, NoteType::Tap, 0 }, { 111600, 0, NoteType::Tap, 0 }, { 111750, 1, NoteType::Tap, 0 }, { 111900, 2, NoteType::Tap, 0 },{ 112050, 3, NoteType::Tap, 0 }, { 112200, 1, NoteType::Tap, 0 }, { 112200, 2, NoteType::Tap, 0 }, { 112800, 0, NoteType::Tap, 0 },{ 112950, 1, NoteType::Tap, 0 }, { 113100, 2, NoteType::Tap, 0 }, { 113400, 3, NoteType::Tap, 0 }, { 114000, 0, NoteType::Tap, 0 },{ 114000, 3, NoteType::Tap, 0 }, { 114600, 1, NoteType::Tap, 0 }, { 114600, 2, NoteType::Tap, 0 }, { 115200, 0, NoteType::Tap, 0 },{ 115500, 1, NoteType::Tap, 0 }, { 115800, 2, NoteType::Tap, 0 }, { 116100, 3, NoteType::Tap, 0 }, { 116400, 0, NoteType::Tap, 0 },{ 116550, 1, NoteType::Tap, 0 }, { 116700, 2, NoteType::Tap, 0 }, { 116850, 3, NoteType::Tap, 0 }, { 117000, 0, NoteType::Tap, 0 },{ 117000, 3, NoteType::Tap, 0 }, { 117600, 1, NoteType::Tap, 0 }, { 117600, 2, NoteType::Tap, 0 }, { 118200, 0, NoteType::Tap, 0 },{ 118800, 3, NoteType::Tap, 0 }, { 119400, 1, NoteType::Tap, 0 }, { 119700, 2, NoteType::Tap, 0 }, { 120000, 0, NoteType::Tap, 0 },{ 120000, 3, NoteType::Tap, 0 }, { 120600, 1, NoteType::Tap, 0 }, { 120900, 2, NoteType::Tap, 0 }, { 121200, 1, NoteType::Tap, 0 },{ 121500, 2, NoteType::Tap, 0 }, { 121800, 0, NoteType::Tap, 0 }, { 122400, 0, NoteType::Tap, 0 }, { 122400, 3, NoteType::Tap, 0 },{ 123000, 1, NoteType::Tap, 0 }, { 123000, 2, NoteType::Tap, 0 }, { 123600, 0, NoteType::Tap, 0 }, { 123750, 1, NoteType::Tap, 0 },{ 123900, 2, NoteType::Tap, 0 }, { 124050, 3, NoteType::Tap, 0 }, { 124200, 0, NoteType::Tap, 0 }, { 124200, 3, NoteType::Tap, 0 },{ 124800, 1, NoteType::Tap, 0 }, { 124950, 2, NoteType::Tap, 0 }, { 125100, 1, NoteType::Tap, 0 }, { 125400, 3, NoteType::Tap, 0 },{ 126000, 0, NoteType::Tap, 0 }, { 126000, 2, NoteType::Tap, 0 }, { 126600, 1, NoteType::Tap, 0 }, { 126600, 3, NoteType::Tap, 0 },{ 127200, 0, NoteType::Tap, 0 }, { 127500, 1, NoteType::Tap, 0 }, { 127800, 2, NoteType::Tap, 0 }, { 128100, 3, NoteType::Tap, 0 },{ 128400, 0, NoteType::Tap, 0 }, { 128400, 1, NoteType::Tap, 0 }, { 129000, 2, NoteType::Tap, 0 }, { 129000, 3, NoteType::Tap, 0 },{ 129600, 0, NoteType::Tap, 0 }, { 129750, 1, NoteType::Tap, 0 }, { 129900, 2, NoteType::Tap, 0 }, { 130050, 3, NoteType::Tap, 0 },{ 130200, 1, NoteType::Tap, 0 }, { 130200, 2, NoteType::Tap, 0 }, { 130800, 0, NoteType::Tap, 0 }, { 130950, 1, NoteType::Tap, 0 },{ 131100, 2, NoteType::Tap, 0 }, { 131400, 3, NoteType::Tap, 0 }, { 132000, 0, NoteType::Tap, 0 }, { 132000, 3, NoteType::Tap, 0 },{ 132600, 1, NoteType::Tap, 0 }, { 132600, 2, NoteType::Tap, 0 }, { 133200, 0, NoteType::Tap, 0 }, { 133500, 1, NoteType::Tap, 0 },{ 133800, 2, NoteType::Tap, 0 }, { 134100, 3, NoteType::Tap, 0 }, { 134400, 0, NoteType::Tap, 0 }, { 134550, 1, NoteType::Tap, 0 },{ 134700, 2, NoteType::Tap, 0 }, { 134850, 3, NoteType::Tap, 0 }, { 135000, 0, NoteType::Tap, 0 }, { 135000, 3, NoteType::Tap, 0 },{ 135600, 1, NoteType::Tap, 0 }, { 135600, 2, NoteType::Tap, 0 }, { 136200, 0, NoteType::Tap, 0 }, { 136800, 3, NoteType::Tap, 0 },{ 137400, 1, NoteType::Tap, 0 }, { 137700, 2, NoteType::Tap, 0 }, { 138000, 0, NoteType::Tap, 0 }, { 138000, 3, NoteType::Tap, 0 },{ 138600, 1, NoteType::Tap, 0 }, { 138900, 2, NoteType::Tap, 0 }, { 139200, 1, NoteType::Tap, 0 }, { 139500, 2, NoteType::Tap, 0 },{ 139800, 0, NoteType::Tap, 0 },{ 141000, 0, NoteType::Tap, 0 }, { 141000, 1, NoteType::Tap, 0 }, { 141000, 2, NoteType::Tap, 0 }, { 141000, 3, NoteType::Tap, 0 }

	};






	for (auto& note : beatmap.notes) {
		++notesTotal;
	}






	while(window.isOpen()){

		int currentSongTimeInMs = beatmap.music.getPlayingOffset().asMilliseconds();


		while(const std::optional event = window.pollEvent()) {



			if(event->is<sf::Event::Closed>()){
				window.close();
			}




			else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){

				if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					switch (mode) {
						case 0:
							window.close();
							break;
						case 1:
							mode = 0;
							beatmap.music.stop();
							break;
						case 2:
							mode = 0;
							break;
					}
				}




				if(mode == 1){
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




		if (mode == 2) {
			if (countdownClock.getElapsedTime().asSeconds() >= 1.0f) {
				countdownSeconds--;
				countdownClock.restart();

				if (countdownSeconds <= 0) {
					mode = 1;
					beatmap.music.play();
				}
				else {
					countdown.setString(std::format("{}", countdownSeconds));
				}
			}
		}








		if (mode == 0) {

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter)){
				startNewGame(beatmap);
				countdown.setString("3");
			}
		}










		for (int i = 0; i < 4; ++i) {
			targetCircles[i].setOutlineColor(sf::Color::Black);
		}



		else if (mode == 1 || mode == 2) {

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





		switch (mode) {


			case 0:
				window.draw(welcomeText)ж
				break;







			case 1:

				scoreText.setString(std::format("Score: {}", score));
				accuracyText.setString(std::format("Accuracy: {}", std::format("{:.2f}", accuracy)));



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








			case 2:
				scoreText.setString(std::format("Score: {}", score));
				accuracyText.setString(std::format("Accuracy: {}%", std::format("{:.2f}", accuracy)));

				for(int i = 0; i < 4; ++i){
					window.draw(targetCircles[i]);
				}
				window.draw(accuracyText);
				window.draw(scoreText);
				window.draw(countdown);


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

	mode = 2;
	countdownSeconds = 3;
	countdownClock.restart();


}
