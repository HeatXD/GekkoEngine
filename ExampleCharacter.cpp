#include "GekkoEngine.h"

std::unique_ptr<CharacterBehaviour> ExampleCharacter;

void BuildBehaviour_ExampleCharacter() {
	ExampleCharacter = std::make_unique<CharacterBehaviour>();
	ExampleCharacter->Init(1, 3, 3);
}


struct CharacterHook_ExampleCharacter {
	CharacterHook_ExampleCharacter() {
		BuildBehaviour_ExampleCharacter();
		Engine::RegisterCharacterBehaviour("ExampleCharacter", ExampleCharacter.get());
	}
};

static CharacterHook_ExampleCharacter characterHook_ExampleCharacter;

std::unique_ptr<CharacterBehaviour> ExampleCharacter2;

void BuildBehaviour_ExampleCharacter2() {
	ExampleCharacter2 = std::make_unique<CharacterBehaviour>();
	ExampleCharacter2->Init(1, 3, 3);
}


struct CharacterHook_ExampleCharacter2 {
	CharacterHook_ExampleCharacter2() {
		BuildBehaviour_ExampleCharacter2();
		Engine::RegisterCharacterBehaviour("ExampleCharacter2", ExampleCharacter2.get());
	}
};

static CharacterHook_ExampleCharacter2 characterHook_ExampleCharacter2;