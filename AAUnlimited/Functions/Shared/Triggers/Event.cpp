#include "Event.h"

#include "Thread.h"

namespace Shared {
namespace Triggers {


	std::wstring g_EventCategories[EVENTCAT_N] = {
		TEXT("Card"),
		TEXT("Character Action"),
		TEXT("Load"),
		TEXT("Time"),
		TEXT("Conversation")
	};

	std::vector<Event> g_Events = {
		{ CLOTHES_CHANGED, EVENTCAT_CHARACTION, TEXT("Clothes Changed"), TEXT("Clothes Changed"),
			TEXT("Whenever a character changes clothes, either in the changing room or by entering the next period."), 
			{}
		},
		{ CARD_INITIALIZED, EVENTCAT_CARD, TEXT("Card Initialized"), TEXT("Card Initialized"),
			TEXT("After the AAU Card data was successfully loaded, either because a class containing this card was loaded, or because "
				 "this card was added to the class roster"),
			{} 
		},
		{ CARD_DESTROYED, EVENTCAT_CARD, TEXT("Card Destroyed"), TEXT("Card Destroyed"),
			TEXT("Before the AAU Card data will be unloaded"),
			{} 
		},
		{ HI_POLY_INIT, EVENTCAT_LOADS, TEXT("Hi Poly Model Initialized"), TEXT("Hi Poly Model Initialized"),
			TEXT("After the Characters High Poly Model was loaded"),
			{}
		},
		{ HI_POLY_END, EVENTCAT_LOADS, TEXT("Hi Poly Model Destroyed"), TEXT("Hi Poly Model Destroyed"),
			TEXT("Before the Characters High Poly Model will be unloaded"),
			{}
		},
		{ CARD_ADDED, EVENTCAT_CARD, TEXT("Card Added to Class"), TEXT("Card Added to Class"),
			TEXT("When a card (including this card) is added to a class, after the CARD_INTIALIZED event is executed"),
			{}
		},
		{ PERIOD_ENDS, EVENTCAT_TIME, TEXT("A Period Ends"), TEXT("A Period Ends"),
			TEXT("After a period ends, including lessions themselves"),
			{}
		},
		{ NPC_RESPONSE, EVENTCAT_CONVERSATION, TEXT("Npc Answers in a Conversation"), TEXT("Npc Answers in a Conversation"),
			TEXT("Whenever a NPC made a yes/no decision."),
			{}
		}
		
	
	};

	void ClothesChangedData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void CardInitializeData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void CardDestroyData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void HiPolyInitData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void HiPolyEndData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void CardAddedData::SetThreadStorage(Thread* thread) const {
		thread->localStorage.triggeringCard = card;
	}

	void PeriodEndsData::SetThreadStorage(Thread* thread) const {
		thread->globalStorage.period = currentPeriod;
	}

	void NpcResponseData::SetThreadStorage(Thread* thread) const {
		
	}
}
}