<!-- split-part | CS2_RESEARCH_MASTER.md lines 127-143 | body-sha256 88a44e8ef8af2ebfa8a111ad37bb4fcdeb019f68ebf23a5544f3d445aaa2c43b -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 6. Health budget, minimum damage и выбор точки

Начало `0x51E4F0` формирует thresholdB из integer entity field и вычитает weight×damage для совпавших target IDs, если weight>0.75. При budget<=0 дальнейшая генерация прекращается.

ThresholdA берётся из context+0x88:

```text
setting <= 100: minimum_score = min(setting, health_budget)
setting > 100:  minimum_score = health_budget
               при mode+0x50==1: ceil(health_budget/2)
затем cap 130
```

Не подменять это распространённым, но не показанным здесь правилом health+(setting−100). Mode+0x50 не называется double-tap без подтверждения связки GUI/state.

`0x51BEA0` сравнивает candidates; `0x524A30` выбирает и дополнительно оптимизирует точку; `0x51C9C0` использует второй sample set и weighted-direction adjustment. При больших массивах используется scheduler, а не только последовательный цикл. Эти C-листинги сохранены полностью, включая неразрешённые типы и условия.
