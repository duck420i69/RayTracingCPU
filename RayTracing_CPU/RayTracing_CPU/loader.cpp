#include "loader.h"

Tga::Tga(const char* FilePath)
{
    std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) { throw std::invalid_argument("File Not Found."); }

    std::uint8_t Header[18] = { 0 };
    std::vector<std::uint8_t> ImageData;
    static std::uint8_t DeCompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    static std::uint8_t IsCompressed[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    hFile.read(reinterpret_cast<char*>(&Header), sizeof(Header));

    if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed)))
    {
        BitsPerPixel = Header[16];
        width = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            hFile.close();
            throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
        }

        ImageData.resize(size);
        ImageCompressed = false;
        hFile.read(reinterpret_cast<char*>(ImageData.data()), size);
    }
    else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed)))
    {
        BitsPerPixel = Header[16];
        width = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            hFile.close();
            throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
        }

        PixelInfo Pixel = { 0 };
        int CurrentByte = 0;
        std::size_t CurrentPixel = 0;
        ImageCompressed = true;
        std::uint8_t ChunkHeader = { 0 };
        int BytesPerPixel = (BitsPerPixel / 8);
        ImageData.resize(width * height * sizeof(PixelInfo));

        do
        {
            hFile.read(reinterpret_cast<char*>(&ChunkHeader), sizeof(ChunkHeader));

            if (ChunkHeader < 128)
            {
                ++ChunkHeader;
                for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                    ImageData[CurrentByte++] = Pixel.b;
                    ImageData[CurrentByte++] = Pixel.g;
                    ImageData[CurrentByte++] = Pixel.r;
                    if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.a;
                }
            }
            else
            {
                ChunkHeader -= 127;
                hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    ImageData[CurrentByte++] = Pixel.b;
                    ImageData[CurrentByte++] = Pixel.g;
                    ImageData[CurrentByte++] = Pixel.r;
                    if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.a;
                }
            }
        } while (CurrentPixel < (width * height));
    }
    else
    {
        hFile.close();
        throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit TGA File.");
    }

    hFile.close();
    this->Pixels = ImageData;
}

Object loadobj(std::string filename) {
    Object newObject;
    std::vector<vec4> vertices;
    std::vector<texel> vertex;
    std::ifstream file;
    file.open(filename);

    if (file.is_open()) {
        bool hasTexture = false;
        bool hasNormal = false;
        while (!file.eof()) {
            std::string str;
            getline(file, str);
            std::stringstream input(str);
            input >> str;
            if (str == "v") {
                vec4 v;
                input >> v.x >> v.y >> v.z; v.w = 1.0f;
                vertices.push_back(v);
            }
            else if (str == "vt") {
                texel temp;
                input >> temp.u >> temp.v;
                vertex.push_back(temp);
            }
            else if (str == "f") {
                char temp;
                int index;
                int v1, v2, v3, vt1, vt2, vt3, trash;
                if (!hasTexture && !hasNormal)
                    input >> v1 >> v2 >> v3;
                if (hasTexture && hasNormal) {
                    input >> v1 >> temp >> vt1 >> temp >> trash;
                    input >> v2 >> temp >> vt2 >> temp >> trash;
                    input >> v3 >> temp >> vt3 >> temp >> trash;
                }
                newObject.mesh.push_back(Triangle(vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1], {1.0f, 1.0f, 1.0f, 1.0f}));
            }
        }
        file.close();
    }

    return newObject;
}
