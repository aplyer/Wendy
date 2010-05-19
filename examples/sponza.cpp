
#include <wendy/Wendy.h>

using namespace wendy;

struct Light
{
  enum Type
  {
    POINT,
    DIRECTIONAL,
  };
  Vec3 direction;
  Vec3 position;
  ColorRGB color;
  float linear;
  Type type;
};

struct Vertex
{
  Vec2 position;
  Vec2 mapping;
  Vec2 clipOverF;
  static VertexFormat format;
};

VertexFormat Vertex::format("2f:position 2f:mapping 2f:clipOverF");

typedef std::vector<Light> LightList;

class Demo : public Trackable
{
public:
  ~Demo(void);
  bool init(void);
  void run(void);
private:
  void renderLight(const Light& light);
  input::SpectatorCamera controller;
  Ref<GL::ImageCanvas> canvas;
  GL::RenderState dirLightPass;
  GL::RenderState pointLightPass;
  GL::RenderState blitPass;
  GL::TextureRef colorTexture;
  GL::TextureRef depthTexture;
  GL::TextureRef normalTexture;
  Ref<render::Camera> camera;
  scene::Graph graph;
  scene::CameraNode* cameraNode;
  LightList lights;
  Timer timer;
  Time currentTime;
};

Demo::~Demo(void)
{
  input::Context::destroy();
  GL::Renderer::destroy();
  GL::Context::destroy();
}

bool Demo::init(void)
{
  Image::addSearchPath(Path("media/sponza"));
  Mesh::addSearchPath(Path("media/sponza"));
  GL::Texture::addSearchPath(Path("media/sponza"));
  GL::VertexProgram::addSearchPath(Path("media/sponza"));
  GL::FragmentProgram::addSearchPath(Path("media/sponza"));
  GL::Program::addSearchPath(Path("media/sponza"));
  render::Material::addSearchPath(Path("media/sponza"));

  if (!GL::Context::create(GL::ContextMode()))
    return false;

  GL::Context* context = GL::Context::get();
  context->setTitle("Sponza Atrium");

  const unsigned int width = context->getCurrentCanvas().getWidth();
  const unsigned int height = context->getCurrentCanvas().getHeight();

  if (!GL::Renderer::create(*context))
    return false;

  if (!input::Context::create(*context))
    return false;

  // Set up G-buffer
  {
    canvas = GL::ImageCanvas::createInstance(*context, width, height);
    if (!canvas)
      return false;

    colorTexture = GL::Texture::createInstance(*context,
                                              Image(PixelFormat::RGBA8, width, height),
                                              GL::Texture::RECTANGULAR,
                                              "colorbuffer");
    if (!colorTexture)
      return false;

    colorTexture->setFilterMode(GL::FILTER_NEAREST);
    canvas->setBuffer(GL::ImageCanvas::COLOR_BUFFER0, &(colorTexture->getImage(0)));

    normalTexture = GL::Texture::createInstance(*context,
                                                Image(PixelFormat::RGBA8, width, height),
                                                GL::Texture::RECTANGULAR,
                                                "normalbuffer");
    if (!normalTexture)
      return false;

    normalTexture->setFilterMode(GL::FILTER_NEAREST);
    canvas->setBuffer(GL::ImageCanvas::COLOR_BUFFER1, &(normalTexture->getImage(0)));

    depthTexture = GL::Texture::createInstance(*context,
                                              Image(PixelFormat::DEPTH32, width, height),
                                              GL::Texture::RECTANGULAR,
                                              "depthbuffer");
    if (!depthTexture)
      return false;

    depthTexture->setFilterMode(GL::FILTER_NEAREST);
    canvas->setBuffer(GL::ImageCanvas::DEPTH_BUFFER, &(depthTexture->getImage(0)));
  }

  // Set up lighting pass
  {
    GL::ProgramRef lightProgram = GL::Program::readInstance("dirlight");
    if (!lightProgram)
      return false;

    GL::ProgramInterface interface;
    interface.addSampler("colorbuffer", GL::Sampler::SAMPLER_RECT);
    interface.addSampler("normalbuffer", GL::Sampler::SAMPLER_RECT);
    interface.addUniform("nearZ", GL::Uniform::FLOAT);
    interface.addUniform("nearOverFarZminusOne", GL::Uniform::FLOAT);
    interface.addUniform("light.direction", GL::Uniform::FLOAT_VEC3);
    interface.addUniform("light.color", GL::Uniform::FLOAT_VEC3);
    interface.addVarying("position", GL::Varying::FLOAT_VEC2);
    interface.addVarying("mapping", GL::Varying::FLOAT_VEC2);
    interface.addVarying("clipOverF", GL::Varying::FLOAT_VEC2);

    if (!interface.matches(*lightProgram, true))
      return false;

    dirLightPass.setBlendFactors(GL::BLEND_ONE, GL::BLEND_ONE);
    dirLightPass.setDepthTesting(false);
    dirLightPass.setDepthWriting(false);
    dirLightPass.setProgram(lightProgram);
    dirLightPass.getSamplerState("colorbuffer").setTexture(colorTexture);
    dirLightPass.getSamplerState("normalbuffer").setTexture(normalTexture);
    dirLightPass.getSamplerState("depthbuffer").setTexture(depthTexture);
  }

  // Set up lighting pass
  {
    GL::ProgramRef lightProgram = GL::Program::readInstance("pointlight");
    if (!lightProgram)
      return false;

    GL::ProgramInterface interface;
    interface.addSampler("colorbuffer", GL::Sampler::SAMPLER_RECT);
    interface.addSampler("normalbuffer", GL::Sampler::SAMPLER_RECT);
    interface.addSampler("depthbuffer", GL::Sampler::SAMPLER_RECT);
    interface.addUniform("nearZ", GL::Uniform::FLOAT);
    interface.addUniform("nearOverFarZminusOne", GL::Uniform::FLOAT);
    interface.addUniform("light.position", GL::Uniform::FLOAT_VEC3);
    interface.addUniform("light.color", GL::Uniform::FLOAT_VEC3);
    interface.addUniform("light.linear", GL::Uniform::FLOAT);
    interface.addVarying("position", GL::Varying::FLOAT_VEC2);
    interface.addVarying("mapping", GL::Varying::FLOAT_VEC2);
    interface.addVarying("clipOverF", GL::Varying::FLOAT_VEC2);

    if (!interface.matches(*lightProgram, true))
      return false;

    pointLightPass.setBlendFactors(GL::BLEND_ONE, GL::BLEND_ONE);
    pointLightPass.setDepthTesting(false);
    pointLightPass.setDepthWriting(false);
    pointLightPass.setProgram(lightProgram);
    pointLightPass.getSamplerState("colorbuffer").setTexture(colorTexture);
    pointLightPass.getSamplerState("normalbuffer").setTexture(normalTexture);
    pointLightPass.getSamplerState("depthbuffer").setTexture(depthTexture);
  }

  // Set up blit pass
  /*
  {
    GL::ProgramRef blitProgram = GL::Program::readInstance("blit");
    if (!blitProgram)
      return false;

    GL::ProgramInterface interface;
    interface.addSampler("image", GL::Sampler::SAMPLER_RECT);
    interface.addVarying("position", GL::Varying::FLOAT_VEC2);
    interface.addVarying("mapping", GL::Varying::FLOAT_VEC2);

    if (!interface.matches(*blitProgram, true))
      return false;

    blitPass.setDepthTesting(false);
    blitPass.setDepthWriting(false);
    blitPass.setProgram(blitProgram);
  }
  */

  Ref<render::Mesh> mesh = render::Mesh::readInstance("sponza");
  if (!mesh)
  {
    Log::writeError("Failed to load mesh");
    return false;
  }

  scene::MeshNode* meshNode = new scene::MeshNode();
  meshNode->setMesh(mesh);
  graph.addRootNode(*meshNode);

  camera = new render::Camera();
  camera->setFOV(60.f);
  camera->setDepthRange(0.9f, 500.f);
  camera->setAspectRatio((float) width / height);

  cameraNode = new scene::CameraNode();
  cameraNode->setCamera(camera);
  graph.addRootNode(*cameraNode);

  lights.resize(1);
  lights[0].position = Vec3::ZERO;
  lights[0].color = ColorRGB::WHITE;
  lights[0].linear = 0.05f;
  lights[0].type = Light::POINT;

  timer.start();

  input::Context::get()->setFocus(&controller);

  return true;
}

void Demo::run(void)
{
  GL::Context* context = GL::Context::get();
  GL::Renderer* renderer = GL::Renderer::get();

  render::Queue queue(*camera);

  do
  {
    if (input::Context::get()->isKeyDown(input::Key::ESCAPE))
      break;

    const Time deltaTime = timer.getTime() - currentTime;
    currentTime += deltaTime;

    lights[0].position.y = sinf(currentTime) * 40.f + 45.f;

    controller.update(deltaTime);
    cameraNode->getLocalTransform() = controller.getTransform();

    graph.update();

    context->setCurrentCanvas(*canvas);
    context->clearDepthBuffer();
    context->clearColorBuffer(ColorRGBA::BLACK);

    graph.enqueue(queue);
    queue.render();
    queue.destroyOperations();

    context->setScreenCanvasCurrent();
    context->clearDepthBuffer();
    context->clearColorBuffer(ColorRGBA::BLACK);

    renderer->setProjectionMatrix2D(1.f, 1.f);

    for (LightList::const_iterator i = lights.begin();  i != lights.end();  i++)
      renderLight(*i);

    /*
    {
      render::Sprite2 sprite;
      sprite.size.set(0.25f, 0.25f);
      sprite.mapping.set(0.25f, 0.25f,
                         0.25f + canvas->getWidth(),
                         0.25f + canvas->getHeight());

      blitPass.getSamplerState("image").setTexture(colorTexture);
      blitPass.apply();

      sprite.position.set(0.f + sprite.size.x / 2.f, 1.f - 0.5f * sprite.size.y);
      sprite.render();

      blitPass.getSamplerState("image").setTexture(normalTexture);
      blitPass.apply();

      sprite.position.set(0.f + sprite.size.x / 2.f, 1.f - 1.5f * sprite.size.y);
      sprite.render();
    }
    */
  }
  while (context->update());
}

void Demo::renderLight(const Light& light)
{
  if (light.type == Light::POINT)
  {
    pointLightPass.getUniformState("nearZ").setValue(camera->getMinDepth());
    pointLightPass.getUniformState("nearOverFarZminusOne").setValue(camera->getMinDepth() / camera->getMaxDepth() - 1.f);

    Vec3 position = light.position;
    camera->getViewTransform().transformVector(position);
    pointLightPass.getUniformState("light.position").setValue(position);

    Vec3 color(light.color.r, light.color.g, light.color.b);
    pointLightPass.getUniformState("light.color").setValue(color);

    pointLightPass.getUniformState("light.linear").setValue(light.linear);

    pointLightPass.apply();
  }
  else if (light.type == Light::DIRECTIONAL)
  {
    dirLightPass.getUniformState("nearZ").setValue(camera->getMinDepth());
    dirLightPass.getUniformState("nearOverFarZminusOne").setValue(camera->getMinDepth() / camera->getMaxDepth() - 1.f);

    Vec3 direction = light.direction;
    camera->getViewTransform().rotation.rotateVector(direction);
    dirLightPass.getUniformState("light.direction").setValue(direction);

    Vec3 color(light.color.r, light.color.g, light.color.b);
    dirLightPass.getUniformState("light.color").setValue(color);

    dirLightPass.apply();
  }

  GL::Renderer* renderer = GL::Renderer::get();

  GL::VertexRange range;

  if (!renderer->allocateVertices(range, 4, Vertex::format))
    return;

  Vertex vertices[4];

  const float radians = camera->getFOV() * (float) M_PI / 180.f;
  const float f = tanf(radians / 2.f);
  const float aspect = camera->getAspectRatio();

  vertices[0].mapping.set(0.5f, 0.5f);
  vertices[0].position.set(0.f, 0.f);
  vertices[0].clipOverF.set(-f * aspect, -f);

  vertices[1].mapping.set(canvas->getWidth() + 0.5f, 0.5f);
  vertices[1].position.set(1.f, 0.f);
  vertices[1].clipOverF.set(f * aspect, -f);

  vertices[2].mapping.set(canvas->getWidth() + 0.5f, canvas->getHeight() + 0.5f);
  vertices[2].position.set(1.f, 1.f);
  vertices[2].clipOverF.set(f * aspect, f);

  vertices[3].mapping.set(0.5f, canvas->getHeight() + 0.5f);
  vertices[3].position.set(0.f, 1.f);
  vertices[3].clipOverF.set(-f * aspect, f);

  range.copyFrom(vertices);

  renderer->render(GL::PrimitiveRange(GL::TRIANGLE_FAN, range));
}

int main()
{
  if (!wendy::initialize())
    exit(1);

  Ptr<Demo> demo(new Demo());
  if (demo->init())
    demo->run();

  demo = NULL;

  wendy::shutdown();
  exit(0);
}

