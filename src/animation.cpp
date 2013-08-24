#include "animation.h"

Animation::Animation() : _texture(NULL), _current_frame(0)
{

}

void Animation::add_frame(sf::IntRect rect)
{
    _frames.push_back(rect);
}

void Animation::set_sprite_sheet(sf::Texture* texture)
{
    _texture = texture;
}

int Animation::get_frame() {
  return _current_frame;
}

void Animation::set_frame(int frame)
{
  _current_frame = frame;
  //calculate new vertex positions and texture coordiantes
  sf::IntRect rect = _frames[_current_frame];

  _vertices[0].position = sf::Vector2f(0, 0);
  _vertices[1].position = sf::Vector2f(0, rect.height);
  _vertices[2].position = sf::Vector2f(rect.width, rect.height);
  _vertices[3].position = sf::Vector2f(rect.width, 0);

  float left = static_cast<float>(rect.left) + 0.0001;
  float right = left + rect.width;
  float top = static_cast<float>(rect.top);
  float bottom = top + rect.height;

  _vertices[0].texCoords = sf::Vector2f(left, top);
  _vertices[1].texCoords = sf::Vector2f(left, bottom);
  _vertices[2].texCoords = sf::Vector2f(right, bottom);
  _vertices[3].texCoords = sf::Vector2f(right, top);
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  states.texture = _texture;
  target.draw(_vertices, 4, sf::Quads, states);
}
