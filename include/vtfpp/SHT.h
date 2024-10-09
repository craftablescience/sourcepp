#pragma once

#include <cstdint>
#include <vector>

namespace vtfpp {

    struct SpriteImagePositions {
        float left;
        float top;
        float right;
        float bottom;
    public:

        [[nodiscard]] float getLeft() const
        {
            return this->left;
        }

        void setLeft(float l)
        {
            this->left = l;
        }

        [[nodiscard]] float getTop() const
        {
            return this->top;
        }

        void setTop(float t)
        {
            this->top = t;
        }


        [[nodiscard]] float getRight() const
        {
            return this->right;
        }

        void setRight(float r)
        {
            this->right = r;
        }

        [[nodiscard]] float getBottom() const
        {
            return this->bottom;
        }

        void setBottom(float b)
        {
            this->bottom = b;
        }

        [[nodiscard]] std::array<float, 4> getPositions() const
        {
            return {this->left,this->top,this->right,this->bottom};
        }

        void setPositions(float l, float t, float r, float b)
        {
            this->left = l;
            this->top = t;
            this->right = r;
            this->bottom = b;
        }

        void setPositions(const float pos[4])
        {
            this->left = pos[0];
            this->top = pos[1];
            this->right = pos[2];
            this->bottom = pos[3];
        }
    };

    //A sprite frame can have 1 or 4 images inside of it. depending on version.
    struct SpriteFrame
    {
        float duration;
        SpriteImagePositions spriteImages[4];
    public:
        void setDuration(float d)
        {
            this->duration = d;
        }

        [[nodiscard]] float getDuration() const
        {
            return this->duration;
        }

        void setSpriteImage(SpriteImagePositions spr1)
        {
            spriteImages[0] = spr1;
            spriteImages[1] = spr1;
            spriteImages[2] = spr1;
            spriteImages[3] = spr1;
        }

        void setSpriteImages(SpriteImagePositions spr1, SpriteImagePositions spr2, SpriteImagePositions spr3, SpriteImagePositions spr4)
        {
            spriteImages[0] = spr1;
            spriteImages[1] = spr2;
            spriteImages[2] = spr3;
            spriteImages[3] = spr4;
        }

        [[nodiscard]] std::array<SpriteImagePositions, 4> getSpriteImages() const
        {
            return {spriteImages[0],spriteImages[1],spriteImages[2],spriteImages[3]};
        }
    };


    struct SpriteSequence {
        uint32_t sequenceNumber;
        uint32_t doesLoop;
//        uint32_t frameCount;
        float totalSequenceTime;
        std::vector<SpriteFrame> frames;
    public:
        [[nodiscard]] uint32_t getSequenceNumber() const
        {
            return sequenceNumber;
        }

        void setSequenceNumber(uint32_t seqNum)
        {
            this->sequenceNumber = seqNum;
        }

        [[nodiscard]] uint32_t getLoop() const
        {
            return doesLoop;
        }

        void setLoop(uint32_t clmp)
        {
            this->doesLoop = clmp;
        }

        [[nodiscard]] uint32_t getTotalSequenceTime() const
        {
            return totalSequenceTime;
        }

        void setTotalSequenceTime(float seqTime)
        {
            this->totalSequenceTime = seqTime;
        }

        void addFrame(SpriteFrame frame)
        {
            this->frames.push_back(frame);
        }

        void removeFrame(uint32_t fra)
        {
            this->frames.erase(this->frames.begin() + fra);
        }

        [[nodiscard]] const std::vector<SpriteFrame>& getFrames() const
        {
            return this->frames;
        }
    };

    class SpriteSheet {
//        uint32_t resourceSize;
        uint32_t version;
//        uint32_t sequenceCount;
        std::vector<SpriteSequence> sequences;

        bool isValid = false;

    public:
        [[nodiscard]] uint32_t getVersion() const
        {
            return version;
        };

        void setVersion(uint32_t v)
        {
            this->version = v;
        }

        [[nodiscard]] const std::vector<SpriteSequence>& getSequences() const
        {
            return sequences;
        }

        void addSequence(SpriteSequence seq)
        {
            this->sequences.push_back(std::move(seq));
        }

        void removeSequence(uint32_t seq)
        {
            this->sequences.erase(this->sequences.begin() + seq);
        }

        [[nodiscard]] uint8_t getSpriteFramePositionCount() const
        {
            return ((this->version != 0) ? 4 : 1);
        }

        SpriteSheet() = default;
        explicit SpriteSheet(const std::vector<std::byte>& data, bool isResource = false);
        SpriteSheet(const unsigned char* data, uint32_t size, bool isResource = false);

        [[nodiscard]] std::vector<std::byte> bake(bool isResource = false) const;

        [[nodiscard]] bool bake(const std::string& path) const;

        [[nodiscard]] explicit operator bool() const;
    };
}
