## ENGR1010J_Project: SVG 解析与 C 语言编辑

### 项目概述

在这个项目中，你将用 C 语言实现一个 SVG 处理系统，该系统能够解析 SVG 文件并将其导出为位图格式。该系统将处理基本的 SVG 元素，并提供命令行和可选的交互式接口。

-----

### 1. 强制任务 (80 分)

#### 1.1 SVG 解析 (40 分)

**核心要求:**

- 实现一个基本的 SVG 解析器，能够读取和解析 SVG 文件。
- 支持至少三种元素类型: `<circle>`、`<rect>` 和 `<line>`。
- 提取和存储几何属性和基本样式属性。
- 在控制台中以清晰、结构化的格式显示解析结果。

**命令行接口:**

```bash
# 解析并显示 SVG 内容
./svg_processor -parse input.svg
./svg_processor p input.svg
```

**预期输出格式:**

```
SVG Document: width = 800.00, height = 600.00
Total shapes: 3
[1] CIRCLE: cx=100.00, cy=200.00, r=50.00, fill=#FF0000
[2] RECT: x=10.00, y=20.00, width=200.00, height=100.00, fill=#00FF00
[3] LINE: from (0.00,0.00) to (150.00,150.00), stroke=#0000FF
```

**实现指导:**

**推荐数据结构:**

创建一个专门的头文件 (`svg_types.h`) 以优化代码组织。

```c
#ifndef SVG_TYPES_H
#define SVG_TYPES_H

typedef enum {
    SVG_SHAPE_CIRCLE,
    SVG_SHAPE_RECT,
    SVG_SHAPE_LINE
} SvgShapeType;

typedef struct {
    double cx, cy, r;
    unsigned int fill_color; // 0xRRGGBB 格式
} SvgCircle;

typedef struct {
    double x, y, width, height;
    unsigned int fill_color; // 0xRRGGBB 格式
} SvgRect;

typedef struct {
    double x1, y1, x2, y2;
    unsigned int stroke_color; // 0xRRGGBB 格式
} SvgLine;

typedef struct SvgShape {
    SvgShapeType type;
    union {
        SvgCircle circle;
        SvgRect rect;
        SvgLine line;
    } data;
    int id;
    struct SvgShape *next; // 用于链表
} SvgShape;

typedef struct {
    double width, height; // SVG 画布尺寸
    SvgShape *shapes; // 形状列表的头部
} SvgDocument;

#endif
```

**解析器接口:**

```c
// 在 svg_parser.h 中
int svg_load_from_file(const char *filename, SvgDocument **doc_out);
/*
 * 读取 SVG 文件并解析：
 * <svg ...> 获取画布尺寸
 * <circle...>, <rect...>, <line...> 获取形状数据
 * 成功返回 0，并设置 *doc_out
 */
void svg_free_document(SvgDocument *doc);
```

**控制台显示:**

```c
// 在 render_console.h 中
void svg_print_summary(const SvgDocument *doc);
void svg_print_shapes(const SvgDocument *doc);
```

**主程序结构 (仅供参考):**

```c
// Example main.c
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input.svg>\n", argv[0]);
        return 1;
    }
    SvgDocument *doc = NULL;
    if (svg_load_from_file(argv[1], &doc) != 0) {
        fprintf(stderr, "Failed to load SVG file: %s\n", argv[1]);
        return 1;
    }
    svg_print_summary(doc);
    svg_print_shapes (doc);
    svg_free_document(doc);
    return 0;
}
```

**实现技巧:**

- 使用简单的字符串函数: `strstr()`、`strchr()`、`sscanf()`。
- 使用 `fgets()` 逐行读取文件。
- 处理 SVG 标签中不同的属性顺序。
- 使用链表高效存储形状。
- 对格式错误的文件实现错误处理。

-----

#### 1.2 BMP/JPG 导出 (40 分)

**核心要求:**

- 将解析后的 SVG 内容导出为 BMP 格式 (必需)。
- 导出为 JPG 格式 (必需)。
- 将矢量形状转换为光栅图像。
- 保持正确的尺寸、宽高比和颜色。

**命令行接口:**

```bash
# 导出为 BMP 格式
./svg_processor -export_bmp input.svg output.bmp
./svg_processor -eb input.svg output.bmp
# 导出为 JPG 格式
./svg_processor -export_jpg input.svg output.jpg
./svg_processor -ej input.svg output.jpg
```

**预期结果:**

- 成功生成 BMP 和 JPG 文件。
- 图像应准确表示 SVG 内容。
- 正确处理形状、颜色和尺寸。

**实现指导:**

**基本方法:**

1. **光栅化:** 将矢量形状转换为基于像素的表示。
2. **颜色处理:** 将 SVG 颜色格式转换为 RGB。
3. **文件格式:** 实现 BMP 和 JPG 文件写入器。

**BMP 导出注意事项:**

- BMP 格式实现起来更简单 - 从这个格式开始。
- 处理 24 位 RGB 格式。
- 实现基本的位图头部结构。
- 使用简单的扫描线渲染。

**JPG 导出注意事项:**

- JPG 需要压缩算法。
- 考虑使用轻量级库 (如果可用)。
- 处理质量设置和压缩。

**光栅化策略 (仅供参考):**

```c
// Example structure
void rasterize_circle(uint8_t *pixels, int width, int height,
                      SvgCircle *circle, uint32_t color) {
    // 实现画圆算法
    // 将圆心坐标和半径转换为像素位置
    // 填充圆边界内的像素
}
```

**实现技巧:**

- 从 BMP 格式开始，因为它更简单。
- 研究基本位图文件结构。
- 一次实现一种形状类型。
- 首先使用简单的 SVG 文件进行测试。
- 考虑使用现有图像写入库进行 JPG 压缩。
- **注意:** 重点在于正确渲染基本形状。在尝试高级功能之前，请先处理圆形、矩形和线条。确切的实现可能因您选择的方法和任何外部库而异。

-----

### 2. 推荐项目结构

虽然您可以将所有代码放在一个 C 文件中，但这被认为是维护性差的**不良实践**。我们推荐以下结构:

```
project/
├── README.md        # 项目描述、构建说明、用法示例
├── Makefile         # 构建配置和编译规则，自动化构建过程、依赖项管理
├── src/             # 源代码文件 (.c)
│   ├── main.c       # 程序入口点，命令行参数处理
│   ├── svg_parser.c # SVG 文件解析实现
│   ├── svg_render.c # 导出函数的渲染逻辑
│   └── bmp_writer.c # BMP 格式导出实现
├── include/         # 头文件 (.h) - 促进模块化
│   ├── svg_parser.h # 解析器函数声明和数据结构
│   ├── svg_render.h # 渲染函数声明
│   └── bmp_writer.h # BMP 导出接口
├── assets/          # 测试 SVG 文件和示例输出，测试文件和资源
├── docs/            # 文档和设计说明
└── build/           # 编译后的对象和可执行文件 (自动生成)
```

**文件职责:**

- **README.md:** 项目概述、构建说明、用法示例。
- **Makefile:** 自动化构建过程、依赖项管理。
- **src/:** 实现代码 (.c 文件)。
- **include/:** 接口定义 (.h 文件) - 促进模块化。
- **assets/:** 测试文件和资源。

-----

### 3. 可选任务 (奖励: 20 分)

对于希望超越基本要求的团队，我们鼓励您实现一个全面的 SVG 编辑系统，允许用户通过交互式命令修改 SVG 文件。这个可选组件将您的解析器转变为一个功能齐全的编辑器，用户可以在其中操作形状、更改属性和创建新图形。

您可以选择以下两种方式之一来实现此编辑功能:

#### 选项 A: 命令行界面 (较简单)

- 在现有命令行基础之上构建。
- 创建一个带有编辑命令的交互式 shell。
- 非常适合熟悉基于终端的应用程序的团队。
- 更容易实现和调试。

#### 选项 B: 图形用户界面 (额外挑战!)

- 创建一个具有实时预览的**可视化应用程序**。
- 实现基于鼠标的交互以进行直接操作。
- 对于对图形编程感兴趣的团队来说是理想的选择。
- 提供更精致的用户体验。

如果您正在寻求一个更具挑战性、能够展示高级编程技能的项目，我们强烈建议尝试图形界面方法！这将为您提供宝贵的图形库和实时应用程序的经验。

-----

#### 3.1 交互式命令行 SVG 编辑器

扩展您的程序，使其具有功能齐全的交互式命令行界面，允许对 SVG 文件进行全面的编辑。所有解析和导出功能应封装为交互式环境中的内部命令。

**启动交互模式:**

```bash
./svg_editor
```

**核心编辑命令:**

- `load <filename>` - 解析 SVG 文件并加载到内存。
- `save [filename]` - 保存已编辑的 SVG 文档。
- `export_bmp <filename>` - 将当前 SVG 导出为 BMP 格式。
- `export_jpg <filename>` - 将当前 SVG 导出为 JPG 格式。
- `add_circle <cx> <cy> <r> [fill]` - 添加新的圆形元素。
- `add_rect <x> <y> <width> <height> [fill]` - 添加新的矩形元素。
- `select <id>` - 选择形状进行编辑。
- `move <dx> <dy>` - 移动选定的形状。
- `delete` - 移除选定的形状。
- `list` - 显示所有形状。

**其他命令 (仅供参考):**

| 命令          | 描述             |
| :------------ | :--------------- |
| `clear`       | 清除所有形状     |
| `zoom`        | 设置缩放级别     |
| `pan`         | 平移视图         |
| `reset`       | 重置视图         |
| `fit`         | 适应内容         |
| `undo`        | 撤销上次操作     |
| `redo`        | 重做操作         |
| `history`     | 显示操作历史记录 |
| `help`        | 显示帮助信息     |
| `status`      | 显示系统状态     |
| `quit`        | 退出程序         |
| `deselect`    | 清除选择         |
| `selected`    | 显示当前选择     |
| `info`        | 显示形状信息     |
| `set_fill`    | 设置填充颜色     |
| `set_stroke`  | 设置描边颜色     |
| `set_width`   | 设置描边宽度     |
| `add_line`    | 添加线条         |
| `add_polygon` | 添加多边形       |

**示例会话 (仅供参考):**

```bash
$ make run
Starting SVG Editor...
[INFO] Starting interactive mode
ENGR1010J SVG Image Processing System
Version 1.0.0
Type 'help' for available commands
Type 'quit' to exit
> help
Available commands:
  load           - Load SVG file
  save           - Save current document
  export         - Export as image
  list           - List all shapes
  info           - Show shape information.
  select         - Select shape by index
  deselect       - Clear selection
  selected       - Show current selection
  move           - Move selected shape
  set_fill       - Set fill color
  set_stroke     - Set stroke color
  set_width      - Set stroke width
  add_circle     - Add circle
  add_rect       - Add rectangle
  add_line       - Add line
  add_polygon    - Add polygon
  delete         - Delete selected shape
  clear          - Clear all shapes
  zoom           - Set zoom level
  pan            - Pan view
  reset          - Reset view
  fit            - Fit to content
  undo           - Undo last operation
  redo           - Redo operation
  history        - Show operation history
  help           - Show help information
  status         - Show system status
  quit           - Exit program
Type 'help <command>' for detailed information about a specific command.
> load examples/basic.svg
[SUCCESS] File loaded: examples/basic.svg (5 shapes)
> list
Shapes in document:
[1] CIRCLE at (150.0,150.0) r=25.0
[2] RECTANGLE at (50.0,75.0) 40.0x30.0
[3] LINE from (0.0,0.0) to (200.0,200.0)
[4] CIRCLE at (300.0,200.0) r=40.0
[5] RECTANGLE at (200.0,300.0) 60.0x40.0
> select 1
[SUCCESS] Selected shape [1]: CIRCLE
> set_fill 255 0 0
[SUCCESS] Fill color set to RGB(255,0,0)
> export output.svg bmp
[SUCCESS] Image exported: output.svg (800x600)
> quit
[INFO] Goodbye!
```

-----

#### 3.2 带有实时预览的图形用户界面

对于更高级的实现，您可以创建一个图形界面，在您编辑 SVG 元素时提供实时视觉反馈。

**推荐库:**

- **SDL2:** (跨平台，对初学者友好)。
- **GTK:** (传统桌面界面)。
- **OpenGL:** (适用于高级图形和性能)。
- **Win32 API:** (Windows 专用解决方案)。

**基本 SDL2 实现指南 (仅供参考):**

```c
// Example structure
#include <SDL2/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SvgDocument* document;
} GuiContext;

void render_svg_to_screen(GuiContext* ctx) {
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
    SDL_RenderClear(ctx->renderer); // 清除渲染器
    
    // 渲染每个形状
    SvgShape* current = ctx->document->shapes;
    while (current != NULL) {
        render_shape(ctx->renderer, current);
        current = current->next;
    }

    SDL_RenderPresent(ctx->renderer); // 更新屏幕
}
```

**要考虑的 GUI 功能:**

- 编辑时实时形状渲染。
- 基于鼠标的形状选择和拖动。
- 可视化属性编辑器 (颜色选择器、尺寸控件)。
- 缩放和平移导航。
- 所有操作的即时视觉反馈。

**示例 GUI 工作流程:**

1. 加载 SVG 文件并进行视觉预览。
2. 在画布上直接单击以选择形状。
3. 拖动手柄以调整大小或移动元素。
4. 使用工具栏按钮或上下文菜单进行操作。
5. 在预览中立即看到更改。

**实现技巧:**

- 从一个简单的只显示查看器开始。
- 添加鼠标交互以进行选择。
- 逐个实现基本的编辑操作。
- 使用 SDL2 的基本绘图函数绘制形状。
- 考虑同时维护 GUI 状态和文档数据。
- **请记住:** 命令行和图形界面都是有效的方法。根据您团队的兴趣和技能进行选择。命令行方法可能更直接实现，而图形界面提供更友好的用户体验。无论您选择哪种方法，都要专注于创建一个**有凝聚力且功能齐全**的编辑环境。

-----

### 4. 使用 Git 进行项目管理

我们强烈建议使用 Git 进行团队协作，而不是通过飞书/微信共享文件。

**为什么要使用 Git?**

- **版本控制:** 跟踪更改并在需要时还原。
- **协作:** 多人可以同时工作而不会发生冲突。
- **备份:** 代码安全地存储在云端。
- **专业实践:** 软件开发的行业标准。

**基本 Git 工作流程:**

1. 一名团队成员创建存储库。
2. 在存储库设置中添加协作者。
3. 使用功能分支 (`feature branches`) 进行新功能开发。
4. 将已完成的功能合并到主分支 (`main branch`)。
5. 频繁提交并附带描述性消息。
