#include <raylib.h>

#include <DesktopCompanion.h>
#include <Utils.h>

int main() {
  // Initialiser le système de journalisation
  InitLogging();

  // Créer et initialiser le compagnon
  DesktopCompanion companion;
  companion.Initialize();

  // Boucle principale
  while (!companion.ShouldClose()) {
    companion.Update();
    companion.Draw();
  }

  // La libération des ressources est gérée par le destructeur
  CloseWindow();
  return 0;
}