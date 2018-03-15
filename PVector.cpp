#ifndef PVECTOR_H
#define PVECTOR_H
#include "Arduino.h"
class PVector {

  public:
    float x, y;
    PVector() {};
    PVector(float _x, float _y) {
      x = _x;
      y = _y;
    };

    void add(PVector v) {
      y = y + v.y;
      x = x + v.x;
    };
    void mult(float n) {
      x = x * n;
      y = y * n;
    };

    void sub(PVector v) {
      x = x - v.x;
      y = y - v.y;
    }

    float mag() {
      return sqrt(x * x + y * y);
    }
    void normalize() {
      float m = mag();
      if (m != 0) {
        div(m);
      }
    }
    void div(float n) {
      x /= n;
      y /= n;

    }
   float magSq() {
      return (x * x + y * y);
    }
    void limit(float max) {
       if (magSq() > max*max) {
      normalize();
      mult(max);
    }
    }
    static PVector sub(PVector v1, PVector v2) {
      return PVector(v1.x - v2.x, v1.y - v2.y);
    }
    static PVector createVector(float _x, float _y) {
      return PVector(_x, _y);
    };
};


#endif