//
// Created by adrien on 06/02/23.
//

#ifndef AUDIOVISUALIZER_LINEOUTSTREAM_H
#define AUDIOVISUALIZER_LINEOUTSTREAM_H

#include <SFML/Audio.hpp>

class LineOutStream : public sf::SoundStream {
    virtual bool onGetData(Chunk& data);

    virtual void onSeek(sf::Time timeOffset);
};


#endif //AUDIOVISUALIZER_LINEOUTSTREAM_H
