```
{
    id: 000001,
    component: { }
}
```

- each ComponentContainer must be able to serialize an entity's component (see above)
- can classes recursively serialize? (e.g. ComponentManager creates a frame and inserts some component's own serialized output as a field)
- how to serialize generic types? 
- if all else fails, implement own serialization library (convert basic types to string and return)

<entities>
    <entity>
        <texture>
            <filename>"box.png"</filename>
        </texture>
        <quad>
            <posX>10</posX>
            <posY>50</posY>
        </quad>
    </entity>
</entities>