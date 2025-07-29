#include <chrono>
#include <raylib.h>
#include <thread>


#include <DesktopCompanion.h>
#include <Utils.h>

int main() {
  // Initialiser le système de journalisation
  InitLogging();

  SetTargetFPS(60); // Limiter à 60 FPS

  // Créer et initialiser le compagnon
  DesktopCompanion companion;
  companion.Initialize();

  // Variables pour gérer le temps
  double previousTime = GetTime();
  float deltaTime = 0.016f; // Valeur par défaut pour la première frame (~60 FPS)

  // Boucle principale
  while (!companion.ShouldClose()) {
    // Calculer deltaTime précis
    double currentTime = GetTime();
    float newDelta = (float)(currentTime - previousTime);
    previousTime = currentTime;

    // Limiter deltaTime pour éviter les saccades en cas de lag ou de pause
    if (newDelta > 0.2f || newDelta <= 0.0f)
      newDelta = 0.016f; // Valeur sûre (~60 FPS)
    deltaTime = newDelta;

    // Mettre à jour avec deltaTime
    companion.Update(deltaTime);
    companion.Draw();

    // Ajout d'une courte pause pour réduire l'utilisation CPU
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  // La libération des ressources est gérée par le destructeur
  CloseWindow();
  return 0;
}