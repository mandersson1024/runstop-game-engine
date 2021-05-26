#include "pch.h"
#include "Easing.h"
#include <numbers>

namespace Engine::Tween
{
    float easeInBack(float t, float b, float c, float d)
    {
        float s = 1.70158f;
        float postFix = t /= d;
        return c * (postFix)*t * ((s + 1) * t - s) + b;
    }

    float easeOutBack(float t, float b, float c, float d)
    {
        float s = 1.70158f;
        return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
    }

    float easeInOutBack(float t, float b, float c, float d)
    {
        float s = 1.70158f;
        if ((t /= d / 2) < 1) return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
        float postFix = t -= 2;
        return c / 2 * ((postFix)*t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
    }

    float easeInBounce(float t, float b, float c, float d)
    {
        return c - easeOutBounce(d - t, 0, c, d) + b;
    }

    float easeOutBounce(float t, float b, float c, float d)
    {
        if ((t /= d) < (1 / 2.75f)) {
            return c * (7.5625f * t * t) + b;
        }
        else if (t < (2 / 2.75f)) {
            float postFix = t -= (1.5f / 2.75f);
            return c * (7.5625f * (postFix)*t + .75f) + b;
        }
        else if (t < (2.5 / 2.75)) {
            float postFix = t -= (2.25f / 2.75f);
            return c * (7.5625f * (postFix)*t + .9375f) + b;
        }
        else {
            float postFix = t -= (2.625f / 2.75f);
            return c * (7.5625f * (postFix)*t + .984375f) + b;
        }
    }

    float easeInOutBounce(float t, float b, float c, float d)
    {
        if (t < d / 2) return easeInBounce(t * 2, 0, c, d) * .5f + b;
        else return easeOutBounce(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
    }

    float easeInCirc(float t, float b, float c, float d)
    {
        return -c * (sqrtf(1 - (t /= d) * t) - 1) + b;
    }

    float easeOutCirc(float t, float b, float c, float d)
    {
        return c * sqrtf(1 - (t = t / d - 1) * t) + b;
    }

    float easeInOutCirc(float t, float b, float c, float d)
    {
        if ((t /= d / 2) < 1) return -c / 2 * (sqrtf(1 - t * t) - 1) + b;
        return c / 2 * (sqrtf(1 - t * (t -= 2)) + 1) + b;
    }

    float easeInCubic(float t, float b, float c, float d)
    {
        return c * (t /= d) * t * t + b;
    }

    float easeOutCubic(float t, float b, float c, float d)
    {
        return c * ((t = t / d - 1) * t * t + 1) + b;
    }

    float easeInOutCubic(float t, float b, float c, float d)
    {
        if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
        return c / 2 * ((t -= 2) * t * t + 2) + b;
    }

    float easeInElastic(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if ((t /= d) == 1) return b + c;
        float p = d * .3f;
        float a = c;
        float s = p / 4;
        float postFix = a * powf(2, 10 * (t -= 1));
        return -(postFix * sinf((t * d - s) * (2 * std::numbers::pi_v<float>) / p)) + b;
    }

    float easeOutElastic(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if ((t /= d) == 1) return b + c;
        float p = d * .3f;
        float a = c;
        float s = p / 4;
        return (a * powf(2, -10 * t) * sinf((t * d - s) * (2 * std::numbers::pi_v<float>) / p) + c + b);
    }

    float easeInOutElastic(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
        float p = d * (.3f * 1.5f);
        float a = c;
        float s = p / 4;

        if (t < 1) {
            float postFix = a * powf(2, 10 * (t -= 1)); // postIncrement is evil
            return -.5f * (postFix * sinf((t * d - s) * (2 * std::numbers::pi_v<float>) / p)) + b;
        }
        float postFix = a * powf(2, -10 * (t -= 1)); // postIncrement is evil
        return postFix * sinf((t * d - s) * (2 * std::numbers::pi_v<float>) / p) * .5f + c + b;
    }

    float easeInExpo(float t, float b, float c, float d)
    {
        return (t == 0) ? b : c * powf(2, 10 * (t / d - 1)) + b;
    }

    float easeOutExpo(float t, float b, float c, float d)
    {
        return (t == d) ? b + c : c * (-powf(2, -10 * t / d) + 1) + b;
    }

    float easeInOutExpo(float t, float b, float c, float d)
    {
        if (t == 0) return b;
        if (t == d) return b + c;
        if ((t /= d / 2) < 1) return c / 2 * powf(2, 10 * (t - 1)) + b;
        return c / 2 * (-powf(2, -10 * --t) + 2) + b;
    }

    float easeNone(float t, float b, float c, float d)
    {
        return c * t / d + b;
    }

    float easeInLinear(float t, float b, float c, float d)
    {
        return c * t / d + b;
    }

    float easeOutLinear(float t, float b, float c, float d)
    {
        return c * t / d + b;
    }

    float easeInOutLinear(float t, float b, float c, float d)
    {
        return c * t / d + b;
    }

    float easeInQuad(float t, float b, float c, float d)
    {
        return c * (t /= d) * t + b;
    }

    float easeOutQuad(float t, float b, float c, float d)
    {
        return -c * (t /= d) * (t - 2) + b;
    }

    float easeInOutQuad(float t, float b, float c, float d)
    {
        if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
        return -c / 2 * (((t - 2) * (--t)) - 1) + b;
    }

    float easeInQuart(float t, float b, float c, float d)
    {
        return c * (t /= d) * t * t * t + b;
    }

    float easeOutQuart(float t, float b, float c, float d)
    {
        return -c * ((t = t / d - 1) * t * t * t - 1) + b;
    }

    float easeInOutQuart(float t, float b, float c, float d)
    {
        if ((t /= d / 2) < 1) return c / 2 * t * t * t * t + b;
        return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
    }

    float easeInQuint(float t, float b, float c, float d)
    {
        return c * (t /= d) * t * t * t * t + b;
    }

    float easeOutQuint(float t, float b, float c, float d)
    {
        return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
    }

    float easeInOutQuint(float t, float b, float c, float d)
    {
        if ((t /= d / 2) < 1) return c / 2 * t * t * t * t * t + b;
        return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
    }

    float easeInSine(float t, float b, float c, float d)
    {
        return -c * cosf(t / d * (std::numbers::pi_v<float> / 2)) + c + b;
    }

    float easeOutSine(float t, float b, float c, float d)
    {
        return c * sinf(t / d * (std::numbers::pi_v<float> / 2)) + b;
    }

    float easeInOutSine(float t, float b, float c, float d)
    {
        return -c / 2 * (cosf(std::numbers::pi_v<float> * t / d) - 1) + b;
    }

}
