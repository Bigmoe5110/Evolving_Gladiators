#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

struct stats{
    enum behavior_t {
        running,
        jumping,
        idle,
        hitting,
        behavior_count
    };
    enum pType_t {
        light,
        base,
        tank,
        ptype_count
    };

    behavior_t behavior; 
    pType_t pType;       

    int attack;
    int defense;
    int speed;
    int health;
    int fitness;
};

void seedRandom()
{
    srand(static_cast<unsigned>(time(NULL)));
}

stats::pType_t randomPType()
{
    return static_cast<stats::pType_t>(rand() % stats::ptype_count);
}

stats::behavior_t randomBehavior()
{
    return static_cast<stats::behavior_t>(rand() % stats::behavior_count);
}

void setStatsForType(stats &p)
{
    if (p.pType == stats::light){
        p.attack = 100;
        p.defense = 50;
        p.speed = 150;
        p.health = 550;
    }
    else if (p.pType == stats::base){
        p.attack = 150;
        p.defense = 100;
        p.speed = 100;
        p.health = 750;
    }
    else if (p.pType == stats::tank){
        p.attack = 200;
        p.defense = 150;
        p.speed = 50;
        p.health = 900;
    }
}

void initPlayer(stats &p, int playerNum)
{
    p.pType = randomPType();
    setStatsForType(p);
    p.fitness = 0;
    if (p.pType == stats::light){
        cout << "Player " << playerNum << " is a light weight with " << p.health << " health." << endl;
    }
    else if (p.pType == stats::base){
        cout << "Player " << playerNum << " is a base with " << p.health << " health." << endl;
    }
    else if (p.pType == stats::tank){
        cout << "Player " << playerNum << " is a tank with " << p.health << " health." << endl;
    }
}

void printBehavior(const stats &p, int playerNum)
{
    if (p.behavior == stats::running){
        cout << "Player " << playerNum << " is running" << endl;
    }
    else if (p.behavior == stats::jumping){
        cout << "Player " << playerNum << " is jumping" << endl;
    }
    else if (p.behavior == stats::idle){
        cout << "Player " << playerNum << " is idle" << endl;
    }
    else if (p.behavior == stats::hitting){
        cout << "Player " << playerNum << " is hitting" << endl;
    }
}

// FORWARD DECLARATIONS - tell C++ these functions exist before they're used
bool handleDodge(const stats &defender, bool randomDodge, int defenderNum);
int handleBlock(const stats &defender, bool randomBlock, int defenderNum);

// Fixed: applyHitIfHitting now properly calculates and applies damage
int applyHitIfHitting(stats &attacker, stats &defender, bool defenderDodgeChance, bool defenderBlockChance, int attackerNum, int defenderNum)
{
    // Not hitting? No damage
    if (attacker.behavior != stats::hitting) {
        return 0;
    }
    
    // Check if defender dodges (dodge prevents all damage)
    if (handleDodge(defender, defenderDodgeChance, defenderNum)) {
        cout << "Player " << attackerNum << "'s attack was dodged by Player " << defenderNum << "!" << endl;
        return 0;
    }
    
    // Calculate base damage
    int dmg = attacker.attack - defender.defense;
    if (dmg < 0) {
        dmg = 0;
    }
    
    // Apply block reduction (returns divisor, 1 = no block)
    int divisor = handleBlock(defender, defenderBlockChance, defenderNum);
    dmg = dmg / divisor;
    
    // Apply the damage
    defender.health -= dmg;
    
    if (dmg > 0) {
        cout << "Player " << attackerNum << " dealt " << dmg << " damage to Player " << defenderNum << "!" << endl;
    }
    
    return dmg;
}

// Fixed: Dodge only depends on random chance and defender's behavior (running helps)
bool handleDodge(const stats &defender, bool randomDodge, int defenderNum)
{
    if (!randomDodge) return false;
    
    // Defender more likely to dodge when running
    if (defender.behavior == stats::running) {
        cout << "Player " << defenderNum << " dodged while running!" << endl;
        return true;
    }
    // Small chance to dodge when jumping
    else if (defender.behavior == stats::jumping && (rand() % 100) < 30) {
        cout << "Player " << defenderNum << " dodged while jumping!" << endl;
        return true;
    }
    
    return false;
}

// Fixed: Block returns divisor based on defense stat
int handleBlock(const stats &defender, bool randomBlock, int defenderNum)
{
    if (!randomBlock) return 1;
    
    // Check defenses from highest to lowest (fixed order)
    if (defender.defense > 150) {
        cout << "Player " << defenderNum << " heavily blocked the attack! (1/6 damage)" << endl;
        return 6;
    }
    else if (defender.defense >= 100) {
        cout << "Player " << defenderNum << " blocked the attack! (1/4 damage)" << endl;
        return 4;
    }
    else if (defender.defense >= 50) {
        cout << "Player " << defenderNum << " partially blocked the attack! (1/2 damage)" << endl;
        return 2;
    }
    
    return 1;
}

// Fixed: Fitness no longer reseeds random and properly adds health-based bonus
void fitnessFunction(stats &p)
{
    // Base fitness is remaining health (higher health = better performance)
    p.fitness = p.health;
    
    // Training bonus (using existing random, not reseeding)
    bool training = (rand() % 100) < 65;
    if (training){
        p.fitness += 10;
        cout << " +10 fitness from training!" << endl;
    }
    
    // Cap fitness to prevent insane growth
    if (p.fitness > 200) p.fitness = 200;
    
    // Apply fitness bonuses to stats
    p.attack += p.fitness / 10;
    p.defense += p.fitness / 10;
    p.speed += p.fitness / 10;
    
    cout << "Fitness: " << p.fitness << " | Stats boosted: +" << p.fitness/10 << " to attack/defense/speed" << endl;
}

void clampHealth(stats &p)
{
    if (p.health < 0){
        p.health = 0;
    }
}

// Fixed: Separate dodge chance calculation from block chance calculation
bool calculateDodgeChance(const stats &player)
{
    if (player.speed >= 150){
        return (rand() % 100) < 75;  // 75% chance
    }
    else if (player.speed >= 100){
        return (rand() % 100) < 25;  // 25% chance
    }
    else {
        return (rand() % 100) < 10;  // 10% chance
    }
}

// Fixed: Block chance based on defense stat with proper thresholds
bool calculateBlockChance(const stats &player)
{
    if (player.defense > 150){
        return (rand() % 100) < 90;  // 90% chance
    }
    else if (player.defense >= 100){
        return (rand() % 100) < 75;  // 75% chance
    }
    else if (player.defense >= 50){
        return (rand() % 100) < 50;  // 50% chance
    }
    return (rand() % 100) < 25;  // 25% chance for defense < 50
}

void runRound(stats &player1, stats &player2)
{
    // Calculate dodge chances for each player (defensive stat)
    bool dodgeChance1 = calculateDodgeChance(player1);
    bool dodgeChance2 = calculateDodgeChance(player2);
    
    // Calculate block chances for each player (defensive stat)
    bool blockChance1 = calculateBlockChance(player1);
    bool blockChance2 = calculateBlockChance(player2);
    
    // Random behaviors
    player1.behavior = randomBehavior();
    player2.behavior = randomBehavior();
    
    cout << "\n--- Round ---" << endl;
    printBehavior(player1, 1);
    printBehavior(player2, 2);
    
    // Apply hits with proper dodge/block checks
    // Note: Defender uses THEIR OWN dodge/block chance
    applyHitIfHitting(player1, player2, dodgeChance2, blockChance2, 1, 2);
    applyHitIfHitting(player2, player1, dodgeChance1, blockChance1, 2, 1);
    
    // Clamp health to 0 minimum
    clampHealth(player1);
    clampHealth(player2);
    
    // Show health status
    cout << "Player 1 health: " << player1.health << " | Player 2 health: " << player2.health << endl;
}

// NEW: Function to reset health before a rematch
void resetHealth(stats &p)
{
    // Restore based on pType
    if (p.pType == stats::light){
        p.health = 550;
    }
    else if (p.pType == stats::base){
        p.health = 750;
    }
    else if (p.pType == stats::tank){
        p.health = 900;
    }
}

int main()
{
    seedRandom();
    stats player1;
    stats player2;
    
    const int GENERATIONS = 5;
    
    for (int gen = 0; gen < GENERATIONS; gen++) {
        cout << "\n========== GENERATION " << gen + 1 << " ==========" << endl;
        
        // Initialize or reinitialize players
        if (gen == 0) {
            initPlayer(player1, 1);
            initPlayer(player2, 2);
        } else {
            // Reset health for rematch (stats carry over from previous fitness)
            resetHealth(player1);
            resetHealth(player2);
            cout << "REMATCH! Stats carried over from previous generation:" << endl;
            cout << "P1: ATT=" << player1.attack << " DEF=" << player1.defense << " SPD=" << player1.speed << endl;
            cout << "P2: ATT=" << player2.attack << " DEF=" << player2.defense << " SPD=" << player2.speed << endl;
        }
        
        // Fight!
        for (int i = 0; i < 10; i++){
            runRound(player1, player2);
            if (player1.health <= 0 || player2.health <= 0){
                break;
            }
        }
        
        // Determine winner and apply fitness
        cout << "\n--- FIGHT RESULT ---" << endl;
        if (player1.health > player2.health){
            cout << "Player 1 wins with " << player1.health << " health remaining!" << endl;
            fitnessFunction(player1);
        }
        else if (player2.health > player1.health){
            cout << "Player 2 wins with " << player2.health << " health remaining!" << endl;
            fitnessFunction(player2);
        }
        else {
            cout << "It's a tie! Both get fitness boost." << endl;
            fitnessFunction(player1);
            fitnessFunction(player2);
        }
    }
    
    // Final champion stats
    cout << "\n========== FINAL CHAMPIONS ==========" << endl;
    cout << "Player 1 - ATT:" << player1.attack << " DEF:" << player1.defense 
         << " SPD:" << player1.speed << " FIT:" << player1.fitness << endl;
    cout << "Player 2 - ATT:" << player2.attack << " DEF:" << player2.defense 
         << " SPD:" << player2.speed << " FIT:" << player2.fitness << endl;
    
    return 0;
}