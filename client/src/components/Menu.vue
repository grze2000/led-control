<template>
  <section class="menu">
    <h2 class="menu__title">LED Control</h2>
    <ul class="menu__list">
      <li class="menu__list-item" v-for="item of menuItems" :key="item.title" @click="item.action()">
        <span :class="['menu__item-icon', item.icon]"></span>
        <span class="menu__item-title">{{ item.title }}</span>
      </li>
    </ul>
  </section>
</template>

<script>
export default {
  name: 'Menu',
  data() {
    return {
      menuItems: [
        {title: 'Sekcja', icon: '', action: () => {this.screen = 'section'}},
        {title: 'Kolor', icon: '', action: () => {this.screen = 'color'}},
        {title: 'Halloween', icon: '', action: this.halloweenMode},
        {title: 'Thanos', icon: '', action: this.thanosSnap}
      ]
    }
  }
    
}
</script>

<style lang="sass" scoped>

</style>