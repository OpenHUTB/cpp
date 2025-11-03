#include "AddMeshFromFile.h"
// add constructor header
#include "UObject/ConstructorHelpers.h"
// Sets default values
AAddMeshFromFile::AAddMeshFromFile()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// add Cylinder to root
    UStaticMeshComponent* Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
    Cylinder->SetupAttachment(RootComponent);
   static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (CylinderAsset.Succeeded())
    {
        Cylinder->SetStaticMesh(CylinderAsset.Object);
        Cylinder->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        Cylinder->SetWorldScale3D(FVector(1.f));
	}
}
// Called when the game starts or when spawned
void AAddMeshFromFile::BeginPlay()
{
	Super::BeginPlay();
}
// Called every frame
void AAddMeshFromFile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}