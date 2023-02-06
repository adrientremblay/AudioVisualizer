//
// Created by adrien on 05/02/23.
//

#include "../include/AbstractMode.h"

#ifndef AUDIOVISUALIZER_WITHFFT_H
#define AUDIOVISUALIZER_WITHFFT_H

/*
const float PI = static_cast<float>(3.14159265358979323846);

using Complex = std::complex<double>;
using ComplAr = std::valarray<Complex>;

class WithFFT : public AbstractMode
{
public:
    WithFFT(sf::SoundBuffer soundBuffer);
    void fft(ComplAr& data);
    void create_hamming_window();

protected:
    void update() override;

    void generate_bars_lr_up(sf::VertexArray& VA,const sf::Vector2f& starting_position);
    void generate_bars_lr_down(sf::VertexArray& VA, const sf::Vector2f& starting_position);

    void generate_line_lr_up(sf::VertexArray& VA, const sf::Vector2f& starting_position);
    void generate_line_lr_down(sf::VertexArray& VA, const sf::Vector2f& starting_position);

    void frequency_spectrum_lr(sf::VertexArray& VA, const sf::Vector2f& starting_position, const int& length, const int& height);
    void frequency_spectrum_round(std::vector<sf::VertexArray>& VAs, std::vector<sf::Color>& colors,const std::vector<float>& heights, const sf::Vector2f& center, const float& radius, const int& from, const int& to);

    void generate_map(sf::VertexArray& VA, const sf::Vector2f& starting_position);

    std::vector<sf::Vertex> map;
    std::vector<Complex> samples;
    ComplAr bin;
    std::vector<float> window;
};

 */
#endif //AUDIOVISUALIZER_WITHFFT_H
