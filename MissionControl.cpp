#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <windows.h>


const int NUM_URLS = 3;
const char URLS[NUM_URLS][64] = {
    "http://fembooru.jp",
    "http://howfeed.biz",
    "http://ff.howfeed.biz"
};
const int REFRESH_INTERVAL = 5;

sf::Text urlLabels[NUM_URLS];
sf::Text statusLabels[NUM_URLS];
bool statuses[NUM_URLS];
sf::Clock timer;


std::string user_friendly_error(int status)
{
    if (status == 1001)
    {
        return "Connection Failed";
    }
    else if (status == 1000)
    {
        return "Invalid Response";
    }
    else
    {
        return std::to_string(status);
    }
}

void refresh()
{
    while (true)
    {
        for (int i = 0; i < NUM_URLS; i++)
        {
            // Create a new HTTP client
            sf::Http http;
            http.setHost(URLS[i]);
            sf::Http::Request request;
            statusLabels[i].setFillColor(sf::Color::White);
            statuses[i] = true;
            statusLabels[i].setString("Loading...");
            // Send the request
            sf::Http::Response response = http.sendRequest(request);
            // Check the status code and display the result
            sf::Http::Response::Status status = response.getStatus();
            statuses[i] = status == sf::Http::Response::Status::Ok;
            statusLabels[i].setString(statuses[i] ? "OK" : "ERROR " + user_friendly_error(status) + "!!!!");
            statusLabels[i].setFillColor(statuses[i] ? sf::Color::Green : sf::Color::Red);
        }
        sf::sleep(sf::seconds(REFRESH_INTERVAL));
    }
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Femboy Mission Control", sf::Style::Titlebar | sf::Style::Close);

    sf::Font vgaFont;
    if (!vgaFont.loadFromFile("vga.ttf"))
    {
        MessageBoxW(NULL, L"WAAAHHHH!!! I can't find my VGA.TTF font!! Vinny!", L"Missing Font", MB_ICONERROR | MB_OK);
        return 1;
    }
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("klaxon.wav"))
    {
        MessageBoxW(NULL, L"WAAAHHHH!!! I can't find my KLAXON.WAV sound!! Just imagine it goes something like this: AWOOOOOOOOGA", L"Missing Sound", MB_ICONERROR | MB_OK);
        return 2;
    }
    sf::Sound klaxon;
    klaxon.setBuffer(buffer);
    sf::Text title;
    title.setFont(vgaFont);
    title.setString("Femboy Mission Control");
    title.setCharacterSize(36);
    sf::Text timerLabel;
    timerLabel.setFont(vgaFont);
    timerLabel.setString("Uptime:");
    timerLabel.setCharacterSize(36);
    timerLabel.setPosition(0, 550);

    for (int i = 0; i < NUM_URLS; i++)
    {
        statuses[i] = true;
        urlLabels[i].setFont(vgaFont);
        statusLabels[i].setFont(vgaFont);
        urlLabels[i].setString(URLS[i]);
        statusLabels[i].setFillColor(sf::Color::White);
        statusLabels[i].setString("Loading...");
        urlLabels[i].setPosition(0, 75 + i * 50);
        statusLabels[i].setPosition(500, 75 + i * 50);
    }

    sf::Thread thread(&refresh);
    thread.launch();

    // for seizure-inducing on/off flashing purposes
    bool even = true;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        window.clear();
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // inside the main loop, between window.clear() and window.display()
        window.draw(title);
        bool flag = false;
        for (int i = 0; i < NUM_URLS; i++)
        {
            window.draw(urlLabels[i]);
            // flashing
            if (!statuses[i])
            {
                statusLabels[i].setFillColor(even ? sf::Color::Red : sf::Color::Yellow);
            }
            window.draw(statusLabels[i]);

            if (!statuses[i])
            {
                flag = true;
            }
        }
        timerLabel.setString("Uptime: " + (flag ? "0.000000" : std::to_string(timer.getElapsedTime().asSeconds())) + " sec");
        window.draw(timerLabel);
        // AWOOOOGA
        if (flag && klaxon.getStatus() != sf::SoundSource::Playing)
        {
            klaxon.play();
            klaxon.setLoop(true);
        }
        else
        {
            klaxon.setLoop(false);
        }
        window.display();
        even = !even;
    }

    thread.terminate();
    return 0;
}
