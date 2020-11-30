
#include <vector>

#include <rdr/renderer.h>
#include <scn/scene.h>

struct rdrImpl;

struct scnImpl
{
    scnImpl();
    ~scnImpl();
    void update(float deltaTime, rdrImpl* renderer);

    void showImGuiControls();

private:
    double time = 0.0;
    std::vector<rdrVertex> vertices;
    float* texture;
    int width;
    int height;
    float scale = 1.f;
};