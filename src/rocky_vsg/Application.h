/**
 * rocky c++
 * Copyright 2023 Pelican Mapping
 * MIT License
 */
#pragma once
#include <rocky_vsg/InstanceVSG.h>
#include <rocky_vsg/MapNode.h>
#include <rocky_vsg/GeoTransform.h>
#include <rocky_vsg/LineState.h>

#include <vsg/app/Viewer.h>
#include <vsg/app/Window.h>
#include <vsg/nodes/Group.h>

#include <list>

namespace ROCKY_NAMESPACE
{
    /**
    * Renderable component that attaches to a MapObject, inheriting its
    * GeoTransform and other shared characteristics.
    */
    class ROCKY_VSG_EXPORT Attachment : public rocky::Inherit<Object, Attachment>
    {
    public:
        bool visible = true;
        vsg::ref_ptr<vsg::Node> node;

    public:
        virtual void createNode(Runtime& runtime) = 0;

    protected:
        Attachment() { }
    };

    using Attachments = std::vector<shared_ptr<Attachment>>;


    /**
    * Interface for the mechanism that will render a particular attachment type.
    */
    class AttachmentRenderer
    {
    };

    using AttachmentRenderers = std::vector<shared_ptr<AttachmentRenderer>>;


    /**
    * An object placed somewhere on the map.
    * A MapObject itself has no visual represetation. You can add attachments
    * to render different things like icons, models, geometries, or text.
    */
    class ROCKY_VSG_EXPORT MapObject : public rocky::Inherit<Object, MapObject>
    {    
    public:
        //! Construct an empty map object.
        MapObject();

        //! Construct a map object with a single attachment.
        MapObject(shared_ptr<Attachment> attachment);

        //! Construct a map object with zero or more attachments.
        MapObject(Attachments attachments);

        //! Globally unique ID for this map object (auto generated)
        const std::uint32_t uid;

        //! Attachments associated with this map object
        Attachments attachments;

        //! Top-level transform for this object (optional)
        vsg::ref_ptr<GeoTransform> xform;
    };


    /**
    * LineString Attachment
    */
    class ROCKY_VSG_EXPORT LineString : public rocky::Inherit<Attachment, LineString>
    {
    public:
        //! Construct a line string attachment
        LineString();

        //! Add a vertex to the end of the line string
        void pushVertex(float x, float y, float z);

        //! Add a vertex to the end of the line string
        template<typename T> void pushVertex(const T& vec3);

        //! Set the rendering style for this line string
        void setStyle(const LineStyle&);

        //! rendering style for the geometry
        const LineStyle& style() const;

    public:
        void createNode(Runtime& runtime) override;

    public:
        vsg::ref_ptr<engine::BindLineStyle> _bindStyle;
        vsg::ref_ptr<engine::LineStringGeometry> _geometry;
    };

    template<typename T> void LineString::pushVertex(const T& vec3) {
        pushVertex(vec3.x, vec3.y, vec3.z);
    }


    /**
    * Polygon attachment
    */
    class ROCKY_VSG_EXPORT Polygon : public rocky::Inherit<Attachment, Polygon>
    {
    public:
        Polygon() { }
    };


    class ROCKY_VSG_EXPORT Icon : public rocky::Inherit<Attachment, Icon>
    {
    public:
        Icon() { }
    };


    class ROCKY_VSG_EXPORT Model : public rocky::Inherit<Attachment, Model>
    {
    public:
        Model() { }
    };


    class ROCKY_VSG_EXPORT Application
    {
    public:
        //! Construct a new application object
        Application(int& argc, char** argv);

        //! Create a main window.
        void createMainWindow(int width, int height, const std::string& name = {});

        //! Access the map.
        shared_ptr<Map> map();

        //! Run until exit.
        int run();

        //! Add a map object to the scene
        void add(shared_ptr<MapObject> object);

        //! Remove a map object from the scene
        void remove(shared_ptr<MapObject> object);

    public:

        rocky::InstanceVSG instance;
        vsg::ref_ptr<rocky::MapNode> mapNode;
        vsg::ref_ptr<vsg::Viewer> viewer;
        vsg::ref_ptr<vsg::Window> mainWindow;
        vsg::ref_ptr<vsg::Group> root;
        vsg::ref_ptr<vsg::Group> mainScene;
        std::function<void()> updateFunction;

    public:
        Application(const Application&) = delete;
        Application(Application&&) = delete;

    protected:
        bool _apilayer = false;
        bool _debuglayer = false;
        bool _vsync = true;
        AttachmentRenderers _renderers;

        struct Addition {
            vsg::ref_ptr<vsg::Node> node;
            vsg::CompileResult compileResult;
        };

        std::mutex _add_remove_mutex;
        std::list<util::Future<Addition>> _additions;
        std::list<vsg::ref_ptr<vsg::Node>> _removals;

        void processAdditionsAndRemovals();
    };
}
