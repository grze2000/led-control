<template>
    <section class="color">
      <h2>Kolor</h2>
      <select id="section" v-model="section">
        <optgroup label="Podstawowe">
          <option value="0">Wszystko</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
          <option value="8">8</option>
          <option value="9">9</option>
        </optgroup>
        <optgroup label="Dodatkowe">
          <option value="10">Przód</option>
          <option value="11">Tył</option>
          <option value="12">Biurko</option>
          <option value="13">Łóżko</option>
          <option value="14">Przód + drzwi</option>
          <option value="15">Tył + środek</option>
        </optgroup>
      </select>
    </section>
</template>

<script>
export default {
    name: 'SectionSelect'
}
</script>

<style lang="sass" scoped>

</style>