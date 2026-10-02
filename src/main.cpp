#include <cstdint>

// Definisi struktur data dasar Bedrock
struct ItemStack;
struct Player;

// Menggunakan C-Linkage sesuai symbol biner native Android
extern "C" {
    // API internal Bedrock yang dibaca dari mod referensi
    ItemStack* Player_getOffhandSlot(Player* player);
    bool ItemStack_isAir(ItemStack* stack);
    int ItemStack_getDescriptorId(ItemStack* stack);
    ItemStack* Inventory_getItem(Player* player, int slot);
    void Player_setOffhandSlot(Player* player, ItemStack* stack);
    void Inventory_setItem(Player* player, int slot, ItemStack* stack);

    // ID Item Totem of Undying di Minecraft Bedrock Engine
    const int TOTEM_ID = 568; 

    // Target Hook Utama: Dipanggil otomatis oleh launcher/game setiap player tick
    __attribute__((visibility("default")))
    void hook_Player_tick(Player* player) {
        if (!player) return;

        // 1. Cek slot Offhand
        ItemStack* offhandItem = Player_getOffhandSlot(player);

        // 2. Jika Offhand kosong atau bukan Totem
        if (!offhandItem || ItemStack_isAir(offhandItem) || ItemStack_getDescriptorId(offhandItem) != TOTEM_ID) {
            
            // 3. Pindai 36 slot inventaris (Hotbar & Main Inventory)
            for (int slot = 0; slot < 36; ++slot) {
                ItemStack* currentItem = Inventory_getItem(player, slot);

                if (currentItem && !ItemStack_isAir(currentItem)) {
                    if (ItemStack_getDescriptorId(currentItem) == TOTEM_ID) {
                        // 4. Pindahkan Totem langsung ke Offhand
                        Player_setOffhandSlot(player, currentItem);
                        break; // Selesai swap dalam 1 tick
                    }
                }
            }
        }
    }
}
