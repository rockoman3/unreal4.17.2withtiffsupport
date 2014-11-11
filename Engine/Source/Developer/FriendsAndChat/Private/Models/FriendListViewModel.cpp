// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FriendsAndChatPrivatePCH.h"
#include "FriendViewModel.h"
#include "FriendsViewModel.h"
#include "FriendListViewModel.h"

class FFriendListViewModelImpl
	: public FFriendListViewModel
{
public:

	~FFriendListViewModelImpl()
	{
		Uninitialize();
	}

	virtual const TArray< TSharedPtr< class FFriendViewModel > >& GetFriendsList() const override
	{
		return FriendsList;
	}

	virtual FText GetListCountText() const override
	{
		return FText::AsNumber(FriendsList.Num());
	}

	virtual int32 GetListCount() const override
	{
		return FriendsList.Num();
	}

	virtual const FText GetListName() const override
	{
		return EFriendsDisplayLists::ToFText(ListType);
	}

	virtual const EFriendsDisplayLists::Type GetListType() const override
	{
		return ListType;
	}

	virtual EVisibility GetListVisibility() const override
	{
		return FriendsList.Num() > 0 ? EVisibility::Visible : EVisibility::Collapsed;
	}

	DECLARE_DERIVED_EVENT(FFriendListViewModelImpl , FFriendListViewModel::FFriendsListUpdated, FFriendsListUpdated);
	virtual FFriendsListUpdated& OnFriendsListUpdated() override
	{
		return FriendsListUpdatedEvent;
	}

private:
	void Initialize()
	{
		FFriendsAndChatManager::Get()->OnFriendsListUpdated().AddSP( this, &FFriendListViewModelImpl::RefreshFriendsList );
		RefreshFriendsList();
	}

	void Uninitialize()
	{
	}

	void RefreshFriendsList()
	{
		FriendsList.Empty();
		TArray< TSharedPtr< IFriendListItems > > OnlineFriendsList;
		TArray< TSharedPtr< IFriendListItems > > OfflineFriendsList;
		TArray< TSharedPtr< IFriendListItems > > FriendItemList;

		if(ListType == EFriendsDisplayLists::GameInviteDisplay)
		{
				// Add Invite items here
				// OfflineFriendsList = 
		}
		else if (ListType == EFriendsDisplayLists::RecentPlayersDisplay)
		{
			OfflineFriendsList = FFriendsAndChatManager::Get()->GetrecentPlayerList();
		}
		else
		{
			FFriendsAndChatManager::Get()->GetFilteredFriendsList( FriendItemList );
			for( const auto& FriendItem : FriendItemList)
			{
				switch (ListType)
				{
					case EFriendsDisplayLists::DefaultDisplay :
					{
						if(FriendItem->GetInviteStatus() == EInviteStatus::Accepted)
						{
							if(FriendItem->IsOnline())
							{
								OnlineFriendsList.Add(FriendItem);
							}
							else
							{
								OfflineFriendsList.Add(FriendItem);
							}
						}
					}
					break;
					case EFriendsDisplayLists::FriendRequestsDisplay :
					{
						if( FriendItem->GetInviteStatus() == EInviteStatus::PendingInbound)
						{
							OfflineFriendsList.Add(FriendItem.ToSharedRef());
						}
					}
					break;
					case EFriendsDisplayLists::OutgoingFriendInvitesDisplay :
					{
						if( FriendItem->GetInviteStatus() == EInviteStatus::PendingOutbound)
						{
							OfflineFriendsList.Add(FriendItem.ToSharedRef());
						}
					}
					break;
				}
			}
		}

		/** Functor for sorting friends list */
		struct FCompareGroupByName
		{
			FORCEINLINE bool operator()( const TSharedPtr< IFriendListItems > A, const TSharedPtr< IFriendListItems > B ) const
			{
				check( A.IsValid() );
				check ( B.IsValid() );
				return ( A->GetName() < B->GetName() );
			}
		};

		OnlineFriendsList.Sort(FCompareGroupByName());
		OfflineFriendsList.Sort(FCompareGroupByName());

		for(const auto& FriendItem : OnlineFriendsList)
		{
			FriendsList.Add(FFriendViewModelFactory::Create(FriendItem.ToSharedRef()));
		}
		for(const auto& FriendItem : OfflineFriendsList)
		{
			FriendsList.Add(FFriendViewModelFactory::Create(FriendItem.ToSharedRef()));
		}

		FriendsListUpdatedEvent.Broadcast();
	}

	FFriendListViewModelImpl(
		const TSharedRef<FFriendsViewModel>& FriendsViewModel,
		EFriendsDisplayLists::Type ListType
		)
		: ViewModel(FriendsViewModel)
		, ListType(ListType)
	{
	}

private:
	const TSharedRef<FFriendsViewModel> ViewModel;
	const EFriendsDisplayLists::Type ListType;

	/** Holds the list of friends. */
	TArray< TSharedPtr< FFriendViewModel > > FriendsList;
	FFriendsListUpdated FriendsListUpdatedEvent;

	friend FFriendListViewModelFactory;
};

TSharedRef< FFriendListViewModel > FFriendListViewModelFactory::Create(
	const TSharedRef<FFriendsViewModel>& FriendsViewModel,
	EFriendsDisplayLists::Type ListType
	)
{
	TSharedRef< FFriendListViewModelImpl > ViewModel(new FFriendListViewModelImpl(FriendsViewModel, ListType));
	ViewModel->Initialize();
	return ViewModel;
}