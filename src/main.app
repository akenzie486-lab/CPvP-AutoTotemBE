#include <cstdint>

struct ItemStack;
struct Player;

extern "C" {
    ItemStack* Player_getOffhandSlot(Player* player);
    bool ItemStack_isAir(ItemStack* stack);
    int ItemStack_getDescriptorId(ItemStack* stack);
    ItemStack* Inventory_getItem(Player* player, int slot);
    void Player_setOffhandSlot(Player* player, ItemStack* stack);
    void Inventory_setItem(Player* player, int slot, ItemStack* stack);

    const int TOTEM_ID = 568; 

    __attribute__((visibility("default")))
    void hook_Player_tick(Player* player) {
        if (!player) return;

        ItemStack* offhandItem = Player_getOffhandSlot(player);

        if (!offhandItem || ItemStack_isAir(offhandItem) || ItemStack_getDescriptorId(offhandItem) != TOTEM_ID) {
            for (int slot = 0; slot < 36; ++slot) {
                ItemStack* currentItem = Inventory_getItem(player, slot);

                if (currentItem && !ItemStack_isAir(currentItem)) {
                    if (ItemStack_getDescriptorId(currentItem) == TOTEM_ID) {
                        Player_setOffhandSlot(player, currentItem);
                        break;
                    }
                }
            }
        }
    }
}
