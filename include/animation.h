#ifndef SCOUNDREL_ANIMATION
#define SCOUNDREL_ANIMATION

// Inspired by https://github.com/LaurentGomila/SFML/wiki/Source%3A-AnimatedSprite

#include <iostream>

#include <vector>
#include <SFML/Graphics.hpp>


class Animation : public sf::Drawable, public sf::Transformable
{
public:
  Animation();
  void add_frame(sf::IntRect rect);
  void set_frame_time(int frame_time);
  void set_sprite_sheet(sf::Texture* texture);
  int get_frame();
  void set_frame(int frame);
  int get_frame_count();
  void increment_frame();
  void setColor(int r=0, int g=0, int b=0, int a=0);

private:
  int _current_frame;
  int _frame_time, _decay;
  sf::Vertex _vertices[4];
  std::vector<sf::IntRect> _frames;
  sf::Color _color;
  const sf::Texture* _texture;
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif
