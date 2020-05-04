// Copyright (c) 2020 Aaron Alberg. All rights reserved.

#include "home_finder_app.h"


namespace homefinderapp {

using cinder::app::KeyEvent;
using cinder::Color;
using cinder::ColorA;
using cinder::Rectf;
using cinder::TextBox;
using cinder::app::KeyEvent;

using std::string;
using std::vector;
using std::stringstream;

using nlohmann::json;

using std::cout;
using std::endl;

const vector<string> kMessages = {
    "Welcome! Select your preferences to find your next home",
    "How important is low crime?",
    "How important is low cost of living?",
    "How important is quality healthcare?",
    "How important is good air quality?",
    "What is your ideal temperature?"};
const string kEndingMessage = "Based on your preferences, you should live in: ";
const Color kThemeColor(0.878, 0.764, 0.956);

#if defined(CINDER_COCOA_TOUCH)
const char kNormalFont[] = "Arial";
const char kBoldFont[] = "Arial-BoldMT";
const char kDifferentFont[] = "AmericanTypewriter";
#elif defined(CINDER_LINUX)
const char kNormalFont[] = "Arial Unicode MS";
const char kBoldFont[] = "Arial Unicode MS";
const char kDifferentFont[] = "Purisa";
#else
const char kNormalFont[] = "Arial";
const char kBoldFont[] = "Arial Bold";
const char kDifferentFont[] = "Papyrus";
#endif

MyApp::MyApp()
  : message_index_{0},
    ended_{false},
    answering_question_{false},
    center{getWindowCenter()},
    city_{"Chicago"},
    answered_{true},
    is_start_{message_index_ == 0},
    current_response_{""} {}


void MyApp::setup() {
  cities_ = ParseJSON();
  cout << cities_[0].name << endl;
  try {
    // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
    http::Request request("http://jsonplaceholder.typicode.com/todos/1");

    // send a get request
    const http::Response response = request.send("GET");

    //writing JSON from request
    json obj;
    stringstream stream;

    stream << std::string(response.body.begin(), response.body.end()) << '\n'; // print the result
    stream >> obj;
    std::cout << obj["title"] << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Request failed, error: " << e.what() << '\n';
  }
}

void MyApp::update() {
  center = getWindowCenter();
  if (!is_start_) {
    answering_question_ = true;
  }

  if (message_index_ == kMessages.size() - 1) {
    ended_ = true;
  }

  if (message_index_ != 0) {
    is_start_ = false;
  }
}

void MyApp::draw() {
  cinder::gl::clear(Color(1,1,1));
  cinder::gl::color(Color(1, 0, 0));
  if (ended_) {
    DrawEnd();
    return;
  }

  DrawMessage();
  DrawButtons();
  if (!current_response_.empty()) DrawCurrentResponse();
  if (answering_question_) DrawDirections();
  if (!answered_ && !is_start_) DrawErrorMessage();
}

//Function courtesy of CS126 Snake MP
template <typename C>
void PrintText(const string& text, const C& color, const cinder::ivec2& size,
               const cinder::vec2& loc) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kNormalFont, 30))
      .size(size)
      .color(color)
      .backgroundColor(ColorA(0, 0, 0, 0))
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2, loc.y - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void MyApp::DrawEnd() {
  PrintText(kEndingMessage, Color(0,0,0),
      {500, 100}, {center.x, center.y - 250});
  PrintText(city_, kThemeColor, {150, 50}, {center.x, center.y});
}

void MyApp::DrawCurrentResponse() {
  PrintText(current_response_ + "%", Color(1,0,0),
      {150,50}, {center.x, center.y});
}

void MyApp::DrawMessage() {
  const cinder::ivec2 size = {500, 120};
  PrintText(kMessages[message_index_],Color(0,0,0),
      size,{center.x, center.y - 250});
}

void MyApp::DrawButtons() {
  DrawNextButton();
  if (is_start_) return; //TODO check this
}

void MyApp::DrawDirections() {
  string directions = "Enter a percentage between 1 and 99";
  PrintText(directions, Color(0,0,0),
      {350, 85},{center.x, center.y - 175});
}

void MyApp::DrawErrorMessage() {
  string error = "You must enter a percentage";
  PrintText(error, Color(1,0,0),
            {350, 85},{center.x, center.y + 300});
}

void MyApp::DrawNextButton() {
  const cinder::ivec2 size = {100, 70};
  cinder::gl::color(Color(.3,.3,.3));
  cinder::gl::drawSolidRect(Rectf(center.x - 50,
      center.y + 155, center.x + 50, center.y + 210));
  PrintText("Next", kThemeColor,
            size, {center.x, center.y + 200});
}


void MyApp::keyDown(KeyEvent event) {
  if (!answering_question_) return;
  int key = event.getChar();

  //ASCII codes representing digits 0 - 9
  if (key >= 48 && key <= 57) {
    if (current_response_.size() >= 2) return;
    current_response_ += event.getChar();
  } else {
    current_response_ = "";
  }
}

void MyApp::mouseDown(cinder::app::MouseEvent event) {
  if (!event.isLeftDown()) return;

  //Next button clicked
  if (event.getX() > center.x - 50 &&
  event.getX() < center.x + 50) {
    if (event.getY() > center.y + 155 && event.getY() < center.y + 210) {
      if (current_response_ == "" && !is_start_) {
        answered_ = false;
        return;
      }

      current_response_ = "";

      //Cap index so it doesn't go out of bounds
      message_index_++;
      answered_ = true;
      if (message_index_ >= kMessages.size()) {
        message_index_ = kMessages.size() - 1;
      }
    }
  }
}
std::vector<mylibrary::City> MyApp::ParseJSON() {

  std::ifstream json_file;
  json_file.open(cinder::app::getAssetPath("population.json").c_str());
  json population_data = json::parse(json_file);

  vector<mylibrary::City> cities;
  for (auto& i : population_data) {
    if (i["population"] == nullptr) {
      break;
    }

    mylibrary::City new_city(i["city_ascii"],
        i["population"],
        i["lat"], i["lng"]);
    cities.push_back(new_city);
  }

  return cities;
}

}  // namespace homefinderapp
