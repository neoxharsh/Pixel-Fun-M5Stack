#ifndef MOVER_H
#define MOVER_H

#include "PVector.cpp"
#include "Arduino.h"
#include <M5Stack.h>
class Mover {
  private:
    float xoff, yoff;
    int width = 320, height = 240, maxh = 0, maxw = 10;

  public:
    int r, g, b;
    PVector location, velocity, mouse, dir;
    M5Display* d;
    Mover(M5Display* _d) {
      location = PVector::createVector(random(width), random(height));
      velocity = PVector::createVector(0, 0);
      
      r = random(0, 255);
      g = random(0, 255);
      b = random(0, 255);
      xoff = 0;
      yoff = 0;
      d = _d;
    };


    void update(int x, int y) {
      mouse  = PVector::createVector(x,y);
      dir = PVector::sub(mouse, location);
      dir.normalize();
      dir.mult(0.5);
      velocity.add(dir);
      velocity.limit(10);
      location.add(velocity);
    };

    void clear(int pixelSize) {

      d->fillRect(location.x, location.y,pixelSize,pixelSize, M5.Lcd.color565(0, 0, 0));
    };

    void display(int pixelSize) {
      d->fillRect(location.x, location.y,pixelSize,pixelSize, M5.Lcd.color565(r, g, b));

    };


    void checkEdges() {
      if (location.x > width) {
        location.x = 0;
      } else if (location.x < 0) {
        location.x = width;
      }
      if (location.y > height) {
        location.y = 0;
      } else if (location.y < 0) {
        location.y = height;
      }
    };

    void bounce() {
      if ((location.x > 128) || (location.x < 0)) {
        velocity.x = velocity.x * -1;
      }
      if ((location.y > 96) || (location.y < 0)) {
        velocity.y = velocity.y * -1;
      }

    }


};


#endif
