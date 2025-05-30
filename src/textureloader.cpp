#include "textureloader.h"

unordered_map<string, Texture2D> TextureLoader::loadedTextures;

Texture2D* TextureLoader::LoadTextureFromFile(const string& filename) {
    if(filename.empty()) return nullptr;

    auto it = loadedTextures.find(filename);
    if (it != loadedTextures.end()) {
        return &it->second; // Return existing texture
    }

    //mayesha
    //E:/1-2 Courses/Structured Programming LAB/Project/projectDemo-master/projectDemo-master/src/assets/Images/ 

    //tausif
    //D:/CSE1202_project/projectDemo-Lab/projectDemo-Lab/src/assets/Images/

    string filepath = "D:/CSE1202_project/projectDemo-Lab/projectDemo-Lab/src/assets/Images/" + filename;
    Texture2D texture = LoadTexture(filepath.c_str());
    if(texture.id != 0){
        loadedTextures[filename] = texture; // Store the loaded texture
        return &loadedTextures[filename];
    } 
    return nullptr; // Return nullptr if loading failed
}

void TextureLoader::DeallocTexture() {
    for (auto& pair : loadedTextures) {
        UnloadTexture(pair.second); // Unload each texture
    }
    loadedTextures.clear(); // Clear the map
}