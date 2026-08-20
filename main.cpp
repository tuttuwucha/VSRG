#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>

/*
todo:
поменять в setPosition width на вычисления в operatableCircleRadius
подумать над isTouching (можно просчитывать высоту летящей ноты)
можно загрузить слайс огурца как текстуру для нот
подумать над движением нот
поменять фон
*/


bool isTouching(sf::CircleShape* operatableCircle){
	return false;
}




int main() {
	unsigned  width = 500;
	unsigned  height = 800;
	sf::RenderWindow window(sf::VideoMode({width, height}), "VSRG");
	window.setFramerateLimit(100);


	sf::Keyboard::Scan circle1Bind = sf::Keyboard::Scan::A;


	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("Sprites/background.png"))
	{
		std::cerr << "Error loading background image!\n";
		return -1;
	}
	sf::Sprite backgroundSprite(backgroundTexture);





	float operatableCircleRadius = 43.0f;

	float operatableCircleHeight = 705.0f;

	sf::CircleShape operatableCircle1(operatableCircleRadius);
	operatableCircle1.setOrigin({operatableCircle1.getRadius(),operatableCircle1.getRadius()});
	operatableCircle1.setFillColor(sf::Color::Transparent);
	operatableCircle1.setOutlineThickness(5.0f);
	operatableCircle1.setPosition({92, operatableCircleHeight});

	sf::CircleShape operatableCircle2(operatableCircleRadius);
	operatableCircle2.setOrigin({operatableCircle2.getRadius(),operatableCircle2.getRadius()});
	operatableCircle2.setFillColor(sf::Color::Transparent);
	operatableCircle2.setOutlineThickness(5.0f);
	operatableCircle2.setPosition({201, operatableCircleHeight});

	sf::CircleShape operatableCircle3(operatableCircleRadius);
	operatableCircle3.setOrigin({operatableCircle3.getRadius(),operatableCircle3.getRadius()});
	operatableCircle3.setFillColor(sf::Color::Transparent);
	operatableCircle3.setOutlineThickness(5.0f);
	operatableCircle3.setPosition({307, operatableCircleHeight});

	sf::CircleShape operatableCircle4(operatableCircleRadius);
	operatableCircle4.setOrigin({operatableCircle4.getRadius(),operatableCircle4.getRadius()});
	operatableCircle4.setFillColor(sf::Color::Transparent);
	operatableCircle4.setOutlineThickness(5.0f);
	operatableCircle4.setPosition({415, operatableCircleHeight});






	std::vector<sf::CircleShape> notes;

	float noteRadius = 43.f;

	int noteCount = 5;

	std::uint8_t noteTransparency = 128;

	sf::Color noteColor(0.f, 0.f, 0.f, noteTransparency);

	for(int i = 0; i < noteCount; ++i){
		sf::CircleShape note(noteRadius);
		note.setOrigin({note.getRadius(), note.getRadius()});
		note.setFillColor(noteColor);
		notes.push_back(note);
	}








	while(window.isOpen()){

		while(const std::optional event = window.pollEvent()) {
			if(event->is<sf::Event::Closed>()){

				window.close();
			}
			else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){

				if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}

			}
		}

		operatableCircle1.setOutlineColor(sf::Color::Black);
		operatableCircle2.setOutlineColor(sf::Color::Black);
		operatableCircle3.setOutlineColor(sf::Color::Black);
		operatableCircle4.setOutlineColor(sf::Color::Black);

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)){
			operatableCircle1.setOutlineColor(sf::Color::Green);
			isTouching(&operatableCircle1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::F)) {
			operatableCircle2.setOutlineColor(sf::Color::Green);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::J)) {
			operatableCircle3.setOutlineColor(sf::Color::Green);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::K)) {
			operatableCircle4.setOutlineColor(sf::Color::Green);
		}







		//render
		window.clear();

		//drawing
		window.draw(backgroundSprite);
		window.draw(operatableCircle1);
		window.draw(operatableCircle2);
		window.draw(operatableCircle3);
		window.draw(operatableCircle4);
		window.draw(notes[0]);

		window.display();
	}


	return 0;
}
