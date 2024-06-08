#include <fstream>
#include <string>
#include <iostream>
#include "reader.hpp"

std::string path = ""; //items.dat path here

std::string cypher = "PBG892FXX982ABC*";

struct item {
	uint32_t itemID;
	std::string name;
};

item* items = NULL;

void decodeItemData(Reader& reader) {
	int version = reader.read<uint16_t>();
	int itemCount = reader.read<uint32_t>();
	items = new item[itemCount];
	if (version > 18) {
		std::cout << "Unsupported items.dat version!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < itemCount; i++) {
		item item;

		item.itemID = reader.read<uint32_t>();
		reader.skip(4);

		int length = reader.read<uint16_t>();
		for (int i = 0; i < length; i++) {
			size_t cypher_char_index = (i + item.itemID) % cypher.length();
			char cypher_char = cypher[cypher_char_index];
			item.name.push_back(reader.read<uint8_t>() ^ cypher_char);
		}

		reader.skipString();
		reader.skip(23);
		reader.skipString();
		reader.skip(8);

		reader.skipStrings(4);

		reader.skip(24);

		reader.skipStrings(3);

		reader.skip(8);
		reader.skip(4);
		reader.skip(68);

		reader.skipString();

		reader.skip(46);
		reader.skipStrings(2);
		reader.skip(8);

		if (i != item.itemID) {
			std::cout << "Unordered item!" << std::endl;
			exit(-1);
		}
		items[i] = item;
	}
	std::cout << "Decoded items.dat" << std::endl;

	std::ofstream outputFile("item_decoded.txt");

	if (outputFile.is_open()) {
		for (int i = 0; i < itemCount; i++) {
			item item = items[i];
			outputFile << "Name: " << item.name << std::endl;
			outputFile << "ItemID: " << item.itemID << "\n" << std::endl;
		}
		std::cout << "Decoded items.dat has been saved to: item_decoded.txt" << std::endl;
		outputFile.close();
	}
	else {
		std::cerr << "Error saving decoded items.dat" << std::endl;
	}
}

void loadItemDataFile(const std::string filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	size_t buffer_size = file.tellg();
	if (buffer_size == -1) {
		std::cout << "Items.dat not found!" << std::endl;
		exit(-1);
	}
	uint8_t* buffer = (uint8_t*)malloc(file.tellg());
	if (!buffer) {
		std::cout << "Something went wrong!" << std::endl;
		exit(-1);
	}
	file.seekg(std::ios::beg);
	file.read((char*)buffer, buffer_size);
	file.close();

	Reader reader = { buffer, buffer_size };

	decodeItemData(reader);

	free(buffer);
}

int main() {
	loadItemDataFile(path);
	std::cout << "\nPress Enter to exit...";
	std::cin.get();
}