
#include "vtfpp/SHT.h"
#include "BufferStream.h"

using namespace vtfpp;

SpriteSheet::SpriteSheet(const std::vector<std::byte> &data, bool isResource) : SpriteSheet(reinterpret_cast<const unsigned char *>(data.data()), data.size(), isResource) {}

SpriteSheet::SpriteSheet(const unsigned char *data, uint32_t size, bool isResource)
{
    BufferStreamReadOnly spriteStream{data,size};

    if(isResource)
        spriteStream.skip<uint32_t>();

    this->version = spriteStream.read<uint32_t>();
    uint32_t sequenceCount = spriteStream.read<uint32_t>();

    if(sequenceCount == 0)
        return;

    for(uint32_t i = 0; i < sequenceCount; i++ )
    {
        auto sequence = &this->sequences.emplace_back();
        sequence->sequenceNumber = spriteStream.read<uint32_t>();
        sequence->doesLoop = spriteStream.read<uint32_t>();
        uint32_t frameCount = spriteStream.read<uint32_t>();
        sequence->totalSequenceTime = spriteStream.read<float>();

        if(frameCount == 0)
            return;

        for(uint32_t j = 0; j < frameCount; j++)
        {
            auto frame = &sequence->frames.emplace_back();
            frame->duration = spriteStream.read<float>();
            for(uint8_t k = 0; k < ((this->version != 0) ? 4 : 1); k++)
            {
                auto spriteImage = &frame->spriteImages[k];
                spriteImage->left = spriteStream.read<float>();
                spriteImage->top = spriteStream.read<float>();
                spriteImage->right = spriteStream.read<float>();
                spriteImage->bottom = spriteStream.read<float>();
            }
        }
    }

   this->isValid = true;
}

SpriteSheet::operator bool() const
{
    return this->isValid;
}

std::vector<std::byte> SpriteSheet::bake(bool isResource) const
{
    if(!this->isValid)
        return {};
    auto sheetData = std::vector<std::byte>();
    BufferStream spriteStream{sheetData };

    if(isResource)
        spriteStream.write<uint32_t>(0); //the resource size is written last;

    spriteStream.write<uint32_t>(this->version);
    spriteStream.write<uint32_t>(this->getSequences().size());

    for(const auto &resource : this->getSequences())
    {
        spriteStream.write<uint32_t>(resource.sequenceNumber);
        spriteStream.write<uint32_t>(resource.doesLoop);
        spriteStream.write<uint32_t>(resource.frames.size());
        spriteStream.write<float>(resource.totalSequenceTime);
        for(const auto &frame : resource.frames)
        {
            spriteStream.write<float>(frame.duration);
            for(uint8_t i = 0; i < ((this->version != 0) ? 4 : 1); i++)
            {
                auto spriteImage = &frame.spriteImages[i];
                spriteStream.write<float>(spriteImage->left);
                spriteStream.write<float>(spriteImage->top);
                spriteStream.write<float>(spriteImage->right);
                spriteStream.write<float>(spriteImage->bottom);
            }
        }
    }

    if(isResource)
    {
        spriteStream.seek(0);
        spriteStream.write<uint32_t>(spriteStream.size());
    }

    sheetData.shrink_to_fit();
    return std::move(sheetData);
}

bool SpriteSheet::bake(const std::string& path) const
{
    return sourcepp::fs::writeFileBuffer(path, this->bake());
}
