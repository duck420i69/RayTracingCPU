#include "loader.h"
#include <iostream>



Tga::Tga(std::filesystem::path FilePath)
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

std::vector<std::shared_ptr<Texture>> loadMTL(std::filesystem::path filename) {
    std::vector<std::shared_ptr<Texture>> data;
    std::ifstream file;
    bool hasMaterial = false;
    file.open(filename);
    if (file.is_open()) {
        Texture newMTL;
        while (!file.eof()) {
            std::string str;
            getline(file, str);
            std::stringstream input(str);
            input >> str;
            if (str == "newmtl") {
                if (hasMaterial) {
                    std::shared_ptr<Texture> texture = std::make_shared<Texture>(std::move(newMTL));
                    newMTL = {};
                    data.push_back(texture);
                }
                hasMaterial = true;
                input >> newMTL.name;
            }
            else if (str == "Ka") {
                for (int i = 0; i < 3; i++) input >> newMTL.ambient_color.v(i);
            }
            else if (str == "Kd") {
                for (int i = 0; i < 3; i++) input >> newMTL.diffuse_color.v(i);
            }
            else if (str == "Ks") {
                for (int i = 0; i < 3; i++) input >> newMTL.specular_color.v(i);
            }
            else if (str == "Ni") {
                input >> newMTL.refraction;
            }
            else if (str == "Ns") {
                input >> newMTL.specular_exp;
            }
            else if (str == "map_Kd") {
                input >> str;
                Tga newTexture(filename.parent_path() / str);
                if (newTexture.HasAlphaChannel()) {
                    newMTL.texture_map.resize(newTexture.GetWidth() * newTexture.GetHeight());
                    std::vector<uint8_t> pixels = newTexture.GetPixels();
                    newMTL.width = newTexture.GetWidth();
                    newMTL.height = newTexture.GetHeight();
                    for (long i = 0; i < newMTL.texture_map.size(); i++) {
                        newMTL.texture_map[i].v(0) = pixels[i * 4 + 2] / 256.0f;
                        newMTL.texture_map[i].v(1) = pixels[i * 4 + 1] / 256.0f;
                        newMTL.texture_map[i].v(2) = pixels[i * 4 + 0] / 256.0f;
                        newMTL.texture_map[i].w = pixels[i * 4 + 3] / 256.0f;
                    }
                }
                else {
                    newMTL.texture_map.resize(newTexture.GetWidth() * newTexture.GetHeight());
                    std::vector<uint8_t> pixels = newTexture.GetPixels();
                    newMTL.width = newTexture.GetWidth();
                    newMTL.height = newTexture.GetHeight();
                    for (long i = 0; i < newMTL.texture_map.size(); i++) {
                        newMTL.texture_map[i].v(0) = pixels[i * 3 + 2] / 256.0f;
                        newMTL.texture_map[i].v(1) = pixels[i * 3 + 1] / 256.0f;
                        newMTL.texture_map[i].v(2) = pixels[i * 3 + 0] / 256.0f;
                        newMTL.texture_map[i].w = 1.0f;
                    }
                }
            }
        }
    }
    return data;
}

Scene loadobj(std::string filename) {
    Object newObject;
    Scene objects;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<vec4> vertices;
    std::vector<texel> vertex;
    std::ifstream file;
    std::filesystem::path path(filename);
    path = std::filesystem::current_path() / path;
    file.open(path);
    if (file.is_open()) {
        int start = 0;
        bool hasTexture = false;
        bool hasNormal = false;
        bool inGroup = false;
        vec4 maximum = { -INFINITY, -INFINITY, -INFINITY, -INFINITY };
        vec4 minimum = { INFINITY, INFINITY, INFINITY, INFINITY };
        while (!file.eof()) {
            std::string str;
            getline(file, str);
            std::stringstream input(str);
            input >> str;
            if (str == "mtllib") {
                input >> str;
                textures = loadMTL(path.parent_path() / str);
                for (auto& tex : textures) {
                    std::cout << tex->name << "\n\n";
                }
            }
            else if (str == "usemtl") {
                input >> str;
                for (auto& tex : textures) {
                    if (tex->name == str) {
                        newObject.texture = tex;
                        break;
                    }
                }
            }
            else if (str == "g") {
                if (inGroup) {
                    newObject.collisionBox.push_back(AABB(minimum, maximum, start, newObject.mesh.size() - 1));
                    objects.objects.push_back(newObject);
                    newObject.mesh.clear();
                    newObject.collisionBox.clear();
                }
                inGroup = true;
                start = newObject.mesh.size();
                maximum = { -INFINITY, -INFINITY, -INFINITY, -INFINITY };
                minimum = { INFINITY, INFINITY, INFINITY, INFINITY };
            }
            else if (str == "v") {
                vec4 v;
                input >> v.v(0) >> v.v(1) >> v.v(2); v.w = 1.0f;
                vertices.push_back(v);
            }
            else if (str == "vt") {
                texel temp;
                input >> temp.x >> temp.y;
                vertex.push_back(temp);
                hasTexture = true;
            }
            else if (str == "vn") {
                hasNormal = true;
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
                for (int i = 0; i < 3; i++) {
                    if (vertices[v1 - 1].v(i) > maximum.v(i)) maximum.v(i) = vertices[v1 - 1].v(i);
                    if (vertices[v1 - 1].v(i) < minimum.v(i)) minimum.v(i) = vertices[v1 - 1].v(i);
                }
                for (int i = 0; i < 3; i++) {
                    if (vertices[v2 - 1].v(i) > maximum.v(i)) maximum.v(i) = vertices[v2 - 1].v(i);
                    if (vertices[v2 - 1].v(i) < minimum.v(i)) minimum.v(i) = vertices[v2 - 1].v(i);
                }
                for (int i = 0; i < 3; i++) {
                    if (vertices[v3 - 1].v(i) > maximum.v(i)) maximum.v(i) = vertices[v3 - 1].v(i);
                    if (vertices[v3 - 1].v(i) < minimum.v(i)) minimum.v(i) = vertices[v3 - 1].v(i);
                }
                if (!hasTexture) newObject.mesh.push_back(Triangle(vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1]));
                else newObject.mesh.push_back(Triangle(vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1], vertex[vt1 - 1], vertex[vt2 - 1], vertex[vt3 - 1]));
            }
        }
        newObject.collisionBox.push_back(AABB(minimum, maximum, start, newObject.mesh.size() - 1));
        objects.objects.push_back(newObject);
        file.close();
    }
    std::cout << "Done loading!\n";
    return objects;
}
