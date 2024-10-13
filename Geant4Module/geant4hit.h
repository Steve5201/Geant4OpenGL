#ifndef GEANT4HIT_H
#define GEANT4HIT_H

#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"

class Geant4Hit : public G4VHit
{
public:
    Geant4Hit();
    virtual ~Geant4Hit();

    // 设置和获取能量沉积
    void SetEnergyDep(G4double edep);
    G4double GetEnergyDep() const;

    // 设置和获取击中位置
    void SetPosition(const G4ThreeVector& pos);
    const G4ThreeVector& GetPosition() const;

    // 设置和获取时间
    void SetTime(G4double t);
    G4double GetTime() const;

    // 重载new和delete
    void* operator new(size_t t);
    void operator delete(void* hit);

private:
    G4double energyDep;         // 能量沉积
    G4ThreeVector position;     // 击中位置
    G4double time;              // 击中时间
};

// 为 G4Hit 定义分配器
extern G4ThreadLocal G4Allocator<Geant4Hit>* Geant4HitAllocator;

inline void *Geant4Hit::operator new(size_t t)
{
    if (!Geant4HitAllocator)
    {
        Geant4HitAllocator = new G4Allocator<Geant4Hit>;
    }
    return (void*)Geant4HitAllocator->MallocSingle();
}

inline void Geant4Hit::operator delete(void *hit)
{
    Geant4HitAllocator->FreeSingle((Geant4Hit*)hit);
}

#endif // GEANT4HIT_H
