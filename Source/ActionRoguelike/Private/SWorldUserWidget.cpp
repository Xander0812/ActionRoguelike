// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing health widget"));

		return;
	}

	FVector2D _screenPosition;
	if(UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, _screenPosition))
	{
		//if (AttachedActor->GetActorLocation()){
			float _scale = UWidgetLayoutLibrary::GetViewportScale(this);

			_screenPosition /= _scale;

			if (ParentSizeBox)
			{
				ParentSizeBox->SetRenderTranslation(_screenPosition);
			}
		//}
	}

}
